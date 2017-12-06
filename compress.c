#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <stdint.h>
#include "compress.h"


/* data structure for each line in the selector table */
typedef struct
{
    uint32_t bits;
    int intstopack;     
    uint32_t masks;
} selector;


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

int number_of_selectors = sizeof(table) / sizeof(*table);


/* return minimum (or first if equal) of two input values */
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
    uint32_t code, shiftedcode;
    
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
    *destination = *destination | which;
    for (current = 0; current < topack; current++) {
        code = raw[current];
        shiftedcode = code << (4 + (current * table[which].bits));
        *destination = *destination | shiftedcode;
    }
    return topack;
}

/* used to use global variable <numints> to keep track of filling decompressed array
   now returns number of ints decompressed */
uint32_t decompress(uint32_t *dest, uint32_t word, int offset)
{
    int i, intsout = 0;
    uint32_t selector, mask, payload, temp;
    selector = word & 0xf;
    mask = table[selector].masks;
    payload = word >> 4;
    for (i = 0; i < table[selector].intstopack; i++) {
        temp = payload & table[selector].masks;
        dest[intsout + offset] = temp;
        intsout++;
        payload = payload >> table[selector].bits;
    }
    return intsout;
}


/* this is adding wasted bits stats to a global array, need to change so it is
   passed a pointer instead */
uint32_t decompress_countwasted(uint32_t *dest, uint32_t word, int offset)
{
    int i, wasted, intsout = 0;
    uint32_t selector, mask, payload, temp;
    selector = word & 0xf;
    mask = table[selector].masks;
    payload = word >> 4;
    for (i = 0; i < table[selector].intstopack; i++) {
        temp = payload & mask;
        wasted = table[selector].bits - fls(temp);
        /* wb[selector].wastedbits += wasted; */
        dest[intsout + offset] = temp;
        intsout++;
        payload = payload >> table[selector].bits;
    }
    /* wb[selector].timesused++; */
    return intsout;
}


