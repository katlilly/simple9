/**********************************************************/
/*  notsimple9.c                                          */
/*  ------------                                          */
/*  A simple9-like compression scheme, using permutations */
/*  of the most common exceptions as well as uniform      */
/*  bitwidth selectors                                    */
/**********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>
#include <math.h>
#include "permutations.h"

#ifndef __APPLE__
#include "fls.h"
#endif

#define NUMBER_OF_DOCS (1024 * 1024 * 1)
#define MAX_BITWIDTH 32
#define MAX_LENGTH 157871
#define NUMBER_OF_LISTS 499693



/* data structure for each line in the selector table */
typedef struct
{
    uint32_t bits;
    int intstopack;     
    uint32_t masks;
} selector;


/* data structure for each line in combination selector table */
typedef struct
{
    int intstopack;
    uint32_t *bits;
} combselector;




/* int selector_26[] = {2,3,2,2,2,2,2}; */

combselector combtable[] =
    {
        {26, NULL},
        {25, NULL},
        {25, NULL},
        {25, NULL},
        {25, NULL},
        {25, NULL},
        {25, NULL},
        {25, NULL},
        {25, NULL},
        {25, NULL},
        {25, NULL},
        {25, NULL},
        {25, NULL},
        {25, NULL},
        {25, NULL},
        {25, NULL},
        {25, NULL},
        {25, NULL},
        {25, NULL},
        {25, NULL},
        {25, NULL},
        {25, NULL},
        {25, NULL},
        {25, NULL},
        {25, NULL},
        {25, NULL},
        {13, NULL},
        {8, NULL},
        {6, NULL},
        {5, NULL},
        {4, NULL},
        {3, NULL},
        {2, NULL},
        {1, NULL}
    };

int number_of_combselectors = sizeof(combtable) / sizeof(*combtable);

/* the selectors for simple-9 */
selector table[] =
    {
        {1,  28, 1},
        {2,  14, 3},
        {3,  9,  7},
        {4,  7,  0xf},
        {5,  5,  0x1f},
        {7,  4,  0x7f},
        {9,  3,  0x1ff},
        {14, 2,  0x3fff},
        {28, 1,  0xfffffff}
    };

uint32_t number_of_selectors = sizeof(table) / sizeof(*table);




uint32_t *postings_list = NULL;
uint32_t *dgaps = NULL;
uint32_t *compressed = NULL;
uint32_t *decoded = NULL;


void print_combtable(combselector *ctable)
{
    int i, j;
    for (i = 0; i < number_of_combselectors; i++) {
        printf("#%d, %d ints, ", i, ctable[i].intstopack);
        for (j = 0; j < ctable[i].intstopack; j++) {
            printf("%d ", ctable[i].bits[j]);
        }
        printf("\n");
    }
}

uint32_t min(uint32_t a, uint32_t b)
{
    return a <= b ? a : b;
}



/* simple 9 compression function */
uint32_t encode(uint32_t *destination, uint32_t *raw, uint32_t integers_to_compress)
{
    uint32_t which;                             /* which element in selector array */
    int current;                                /* count of elements within each compressed word */
    int topack;                                 /* min of intstopack and what's available to compress */
    uint32_t *integer = raw;                    /* the current integer to compress */
    uint32_t *end = raw + integers_to_compress; /* the end of the input array */
    
    /* chose selector */
    for (which = 0; which < number_of_selectors; which++)
        {
            topack = min(integers_to_compress, table[which].intstopack);
            end = raw + topack;
            for (; integer < end; integer++) {
                if (fls(*integer) > table[which].bits)
                    break; 
            }
            if (integer >= end) {
                break;
            }
        }

    /* pack one word */
    *destination = 0;
    uint32_t code;
    *destination = *destination | which;
    for (current = 0; current < topack; current++) {
        code = raw[current];
        uint32_t shiftedcode = code << (4 + (current * table[which].bits));
        *destination = *destination | shiftedcode;
    }
    return topack;    /* return number of dgaps compressed into this word */
}


/* not-simple-9 compression function, using selector set with exceptions in them */
uint32_t encode_excp(uint32_t *destination, uint32_t *raw, uint32_t integers_to_compress)
{
    uint32_t which;                             /* which row in selector array */
    int column;                                 /* which element in bitwidth array */
    int current;                                /* count of elements within each compressed word */
    int topack;                                 /* min of ints/selector and remaining data to compress */
    uint32_t *integer = raw;                    /* the current integer to compress */
    uint32_t *end = raw + integers_to_compress; /* the end of the input array */
    
    /* choose selector */
    uint32_t *start = integer;
    for (which = 0; which < number_of_combselectors; which++)
        {
	  column = 0; /* go back to start of each row because of way some selectors may be ordered */
	  integer = start; /* and also go back to first int that needs compressing */
            topack = min(integers_to_compress, combtable[which].intstopack);
            end = raw + topack;
            for (; integer < end; integer++) {
                if (fls(*integer) > combtable[which].bits[column]) {
		  break; /* increment 'which' if current integer can't fit in this many bits */
                }
                column++;
            }
            if (integer >= end) {
                break;
            }
        }
    
    /* pack one word */
    *destination = 0;
    *destination = *destination | which; /* put selector in (still using 4 bits for now) */
    int i = 0;
    int shiftdistance = 4;
    for (current = 0; current < topack; current++) {
        *destination = *destination | raw[current] << shiftdistance;
        shiftdistance += combtable[which].bits[i];
        i++;
    }
    return topack;   /* return number of dgaps compressed into this word */
}


/* returns number of ints decompressed */
uint32_t decompress(uint32_t *dest, uint32_t word, int offset)
{
    int i, intsout = 0;
    uint32_t selector, mask, payload, temp;
    selector = word & 0xf;
    mask = table[selector].masks;
    payload = word >> 4;
    for (i = 0; i < table[selector].intstopack; i++) {
        temp = payload & table[selector].masks;
        decoded[intsout + offset] = temp;
        intsout++;
        payload = payload >> table[selector].bits;
    }
    return intsout;
}

/* decompression with non-uniform selectors */
uint32_t decompress_excp(uint32_t *dest, uint32_t word, int offset)
{
    int i, bits, intsout = 0;
    uint32_t selector, mask, payload;
    selector = word & 0xf; /* note still using 4 bit selector for now */
    payload = word >> 4;
    for (i = 0; i < combtable[selector].intstopack; i++) {
        bits = combtable[selector].bits[i];
        mask = pow(2, bits) - 1;
	decoded[intsout + offset] = payload & mask;
        intsout++;
        payload = payload >> bits;
    }
    return intsout;
}




int main(void)
{
    /* print bespoke selector table for a list given statistical description of data */
    /* ************************************ */
    /* int numperms = 0; */
    /* int mode = 1; */
    /* int spread = 1; */
    /* int exceptionfreq = 1; */
    /* make_selector_table(mode, spread, exceptionfreq); */
    /* printf("number of permutations: %d\n", numperms); */

    int listnumber, payload_bits, excp_perms, uniform_selectors, num_selectors;
    uint32_t i, prev, length, bitwidth;
    uint32_t compressedwords;
    uint32_t compressedints;

    listnumber = 0;
    payload_bits = 26;
    excp_perms = 25;
    uniform_selectors = 9;
    num_selectors = excp_perms + uniform_selectors;

    printf("num selectors: %d\n", number_of_combselectors);

    /* set bitwidth arrays for uniform selectors */
    for (i = 0; i < number_of_combselectors; i++) {
        combtable[i].bits = malloc(combtable[i].intstopack * sizeof(combtable[i].bits[0]));
        for (int j = 0; j < combtable[i].intstopack; j++) {
            combtable[i].bits[j] = 26 / combtable[i].intstopack;
        }
    }

    /* set the exceptions, in this case they are in rows 1 to 25 */
    for (i = 1; i < 26; i++) {
        combtable[i].bits[25-i] = 2;
    }

    
    /* for (i = 0; i < num_selectors; i++) { */
    /*     combtable[i].bits = malloc(combtable[i].intstopack * sizeof combtable[i].bits[0]); */
    /*     bitwidth = 26 / combtable[i].intstopack; */
    /*     printf("i: %d, bitwidth: %d\n", i, bitwidth); */
    /*     memset(combtable[i].bits, bitwidth, combtable[i].intstopack * sizeof (combtable[i].bits[0])); */
    /* } */

    print_combtable(combtable);

    return 0;
}
