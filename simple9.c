#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <stdint.h>
//#include "fls.h"


#define NUMBER_OF_DOCS (1024 * 1024 * 1)
#define MAX_BITWIDTH 32
#define MAX_LENGTH 157871
#define NUMBER_OF_LISTS 499693


/* data structure for each line in the selector table */
typedef struct
{
    uint32_t bits;
    int intstopack;     // number of ints to pack
    uint32_t masks;
} selector;


/* data structure for selector use statistics */
typedef struct
{
    int selector;
    int frequency;
} stats;


/* data stucture for recording statistics for a single postings list */
typedef struct
{
    int listlength;
    int numcompressedwords;
    int wastedbits;
} ratios;


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


/* array of stats structs to record selector use statistics for entire dataset,
 initialise freqs to zero */
stats selectorfreqs[] =
{
    {1, 0},
    {2, 0},
    {3, 0},
    {4, 0},
    {5, 0},
    {7, 0},
    {9, 0},
    {14, 0},
    {28, 0}
};


ratios *cr = NULL;
uint32_t *postings_list = NULL;
uint32_t *dgaps = NULL;
uint32_t *compressed = NULL;
uint32_t *decoded = NULL;


/* print an unsigned 32 bit int in big-endian binary */
void print_binary(uint32_t num)
{
    int i;
    for (i = 31; i >= 0; i--) {
        if (num & (1<<i)) {
            printf("1");
        } else {
            printf("0");
        }
    }
    printf("\n");
}


uint32_t min(uint32_t a, uint32_t b)
{
    return a <= b ? a : b;
}


/* simple 9 compression function */
uint32_t encode(uint32_t *destination, uint32_t *raw, uint32_t integers_to_compress)
{
    uint32_t which;                             // which element in selector array
    int current;                                // count of elements within each compressed word
    int topack;                                 // min of selector's intstopack and what's left to compress
    uint32_t *integer = raw;                    // the current integer to compress
    uint32_t *end = raw + integers_to_compress; // the end of the input array
    
    /* chose selector */
    for (which = 0; which < number_of_selectors; which++)
    {   // start by assuming we can fit 28 ints in
        topack = min(integers_to_compress, table[which].intstopack);
        end = raw + topack;
        //end = raw + min(integers_to_compress, table[which].intstopack);
        for (; integer < end; integer++) {
            if (fls(*integer) > table[which].bits)
                break; // increment 'which' if current integer can't fit in this many bits
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
    
//    *destination = 0; // initialize word to zero before packing ints and selector into it
//    for (current = 0; current < table[which].intstopack; current++) {
//        uint32_t value = current > integers_to_compress ? 0 : raw[current];
//        // value of current int (or pack some zeros in last word if we're at end of list)
//        *destination = *destination << table[which].bits | value; // pack ints into compressed word
//        // print_binary(*destination);
//    }
//    *destination = *destination << 4 | which; // put the selector into compressed word
//    print_binary(*destination);
    
    return topack;    // return number of dgaps compressed so far.
}

// chose selector: 9, for up to 3 ints, will encode 3 ints
// 00000 000000000 000000000 010011010
// 00000 000000000 010011010 001010000
// 00000 010011010 001010000 001011001
// compressed word with selector added:
// 0 010011010 001010000 001011001 0110


/* used to use global variable <numints> to keep track of filling decompressed array
 now returns number of ints decompressed */
uint32_t decompress(uint32_t *dest, uint32_t word, int offset)
{
    int i, intsout = 0;
    uint32_t selector, mask, payload, temp;
    selector = word & 0xf;
    mask = (1 << (selector)) - 1;
    //printf("payload:\n");
    payload = word >> 4;
    //print_binary(payload);
    for (i = 0; i < table[selector].intstopack; i++) {
        temp = payload & table[selector].masks;// << (table[selector].bits * i));
        
        decoded[intsout + offset] = temp;
        intsout++;
        payload = payload >> table[selector].bits;
        //printf("current state of payload: ");
        //print_binary(payload);
    }
//    for (i = table[selector].intstopack; i > 0; i--) {
//        mask = table[selector].masks << (table[selector].bits * i);
//        temp = payload & mask;
//
//        decompressed[index++] = temp;
//        intsout++;
//        payload = payload >> table[selector].bits;
//        //printf("current state of payload: ");
//        //print_binary(payload);
//    }
    //printf("\n");
    //printf("decompressed %d ints\n", intsout);
    return intsout;
}


uint32_t decode(uint32_t *destination, uint32_t word) {
    uint32_t selector = word & 0xf;
    printf("selector: %d\n", table[selector].bits);
    int bits = 0;
    for (int i = 0; i < number_of_selectors; i++) {
        if (selector == table[i].bits)
        {
            bits = table[i].bits;
        }
    }
    uint32_t numdecompressed = table[bits].intstopack;
    uint32_t payload = word >> 4;
    for (int i = 0; i < table[selector].intstopack; i++) {


    }

    return numdecompressed;
}


int compare_ints(const void *a, const void *b) {
    const int *ia = (const int *) a;
    const int *ib = (const int *) b;
    return *ia < *ib ? -1 : *ia == *ib ? 0 : 1;
}


/* this function repeats most of what the decompression function does */
int countwastedbits(uint32_t word) {
    int i;
    uint32_t selector, mask, payload, temp, leadingzeros = 0;
    selector = word & 0xf;
    selector = table[selector].bits;
    //printf("selector: %d\n", selector);
    //selectorfreqs[selector]++;
    mask = (1 << (selector)) - 1;
    payload = word >> 4;
    for (i = 0; i < (28/selector) ; i++) {
        temp = payload & mask;
        payload = payload >> selector;
        leadingzeros += selector - fls(temp);
    }
    return leadingzeros;
}


/* function for making synthetic data that will compress in a known way */
uint32_t * makefakedata(uint32_t *dest, int number, int numberofnumbers) {
    int i;
    for (i = 0; i < numberofnumbers; i++) {
        dest[i] = number;
    }
    return dest;
}


int main(int argc, char *argv[]) {
    int listnumber = 0;
    int wastedbits;
    uint32_t i, j, prev, length;
    uint32_t compressedwords;
    uint32_t compressedints;

    const char *filename;
    if (argc == 2) {
        filename = argv[1];
    } else {
        exit(printf("Usage::%s <binfile>\n", argv[0]));
    }
    //printf("Using: %s\n", filename);

    postings_list = malloc(NUMBER_OF_DOCS * sizeof *postings_list);
    dgaps = malloc(NUMBER_OF_DOCS * sizeof *dgaps);
    compressed = malloc(NUMBER_OF_DOCS * sizeof *compressed);
    decoded = malloc(NUMBER_OF_DOCS * sizeof *decoded);
    cr = malloc(NUMBER_OF_LISTS * sizeof *cr);
    
    
    /* an array for storing bit width statistics of ints to be compressed */
    int *bitwidths = malloc(MAX_BITWIDTH * sizeof *bitwidths);
    for (i =0; i < MAX_BITWIDTH; i++) {
        bitwidths[i] = 0;
    }

    /* set up an array for keeping track of list length statistics */
    int *lengthfreqs = malloc(MAX_LENGTH * sizeof *lengthfreqs);
    memset(lengthfreqs, 0, MAX_LENGTH * sizeof *lengthfreqs);

    FILE *fp;
    if ((fp = fopen(filename, "rb")) == NULL) {
        exit(printf("Cannot open %s\n", filename));
    }

    /* read in postings list and compress each one */
    while (fread(&length, sizeof(length), 1, fp)  == 1) {
        
        /* Read one postings list (and make sure we did so successfully) */
        if (fread(postings_list, sizeof(*postings_list), length, fp) != length) {
            exit(printf("i/o error\n"));
        }
        listnumber++;
        
        
        /* print current postings list */
        // ***************************
//        printf("%u: ", (unsigned)length);
//        for (uint32_t *where = postings_list; where < postings_list + length; where++) {
//            printf("%u ", (unsigned)*where);
//        }
//        printf("\n");

        /* add this list length to tally of list length frequencies */
        //printf("list number: %d, list length: %d\n", listnumber, length);
        lengthfreqs[length]++;
        
        
        /* convert postings list to dgaps list */
        prev = 0;
        for (i = 0; i < length; i++) {
            dgaps[i] = postings_list[i] - prev;
            prev = postings_list[i];
        }
        
        /* print dgap lists */
        // ****************
//        printf("%d: ", length);
//        for (i = 0; i < length; i++) {
//            printf("%d, ", dgaps[i]);
//        }
//        printf("\n");
        
        
        /* an array for storing bit width statistics for a single list */
        int *single_list_bitwidths = malloc(MAX_BITWIDTH * sizeof *single_list_bitwidths);
        for (i = 0; i < MAX_BITWIDTH; i++) {
            single_list_bitwidths[i] = 0;
        }
        
        /* count bits needed for each dgap, both for a single list and
         as cumulative stats for entire set of lists */
        int bitwidth;
        for (i = 0; i < length; i++) {
            bitwidth = fls(dgaps[i]);
            bitwidths[bitwidth]++;
            single_list_bitwidths[bitwidth]++;
        }
        
        /* print bitwidth statistics for a single list */
        // *******************************************
//        printf("Bitwidth stats for %dth list: \n", listnumber);
//        for (i = 0; i < MAX_BITWIDTH; i++) {
//            printf("%d, %d\n", i, single_list_bitwidths[i]);
//        }
//        free(single_list_bitwidths);

        
        /* compress this postings list */
        uint32_t numencoded = 0;  // return value of encode function, the number of ints compressed
        compressedwords = 0;      // offset for position in output array "compressed"
        compressedints = 0;       // offset for position in input array "dgaps"
        for (compressedints = 0; compressedints < length; compressedints += numencoded) {
            numencoded = encode(compressed + compressedwords, dgaps + compressedints, length - compressedints);
            compressedwords++;
        }
        
       
        
        /* add selector freqs for this compressed list to global array of selector freqs */
        // *****************************************************************************
        for (j = 0; j < compressedwords; j++) {
            int selector = compressed[j] & 0xf;
            //if (compressedints > 100) { // only look at 'long' lists
                selectorfreqs[selector].frequency += 1;
            //}
            //printf("selector %d\n", table[selector].bits);
        }

        /* add compression ratio stats for this list */
        // *****************************************
        cr[listnumber].listlength = length;
        cr[listnumber].numcompressedwords = compressedwords;

        /* count wasted bits in entire compressed list */
        // ****** not right *********
        wastedbits = 0;
        // don't look at last word because it might not be full
        if (length > 100) {
            for (i = 0; i < compressedwords - 1; i++) {
                //print_binary(compressed[i]);
                wastedbits += countwastedbits(compressed[i]);
                //qprintf("wasted bits in %dth word: %d\n", i, wastedbits);
            }
        }
        // then add total internally wasted bits for this list to cr[listnumber].wastedbits

        /* decompress the current postings list */
        // ************************************
        int offset = 0;  // reset number of decompressed ints to zero for each word
        for (i = 0; i < compressedwords; i++) {
            offset += decompress(decoded, compressed[i], offset);
        }
        
        
        /* print decompressed dgaps list */
        // *****************************
//        printf("decompressed: %d\n", length);
//        for (i = 0; i < length; i++) {
//            printf("%u, ", decoded[i]);
//        }
//        printf("\n\n");
        
        
        /* find errors in compression or decompression */
        // *******************************************
//        printf("original: decompressed:\n");
//        for (i = 0; i < length; i++) {
//            printf("%6d        %6d", dgaps[i], decoded[i]);
//            if (dgaps[i] != decoded[i]) {
//                printf("     wrong");
//            }
//            printf("\n");
//        }
   
    }// end read-in of postings lists

    printf("number of lists: %d\n", listnumber);
    
    
    /* print bitwidth stats array to export to matlab */
    //*************************************************
//    for (i = 0; i < MAX_BITWIDTH; i++) {
//        printf("%d, %d\n", i, bitwidth_stats[i]);
//    }


    /* print compression ratios */
    //**********************************
//    for (i = 0; i < numberoflists; i++) {
//        if (cr[i].listlength > 100) {
//            double compressed = (double) cr[i].numcompressedwords;
//            double raw = (double) cr[i].listlength;
//            printf("list number: %7d, list length: %2d, compressed length: %2d, compression ratio: %f\n", i, cr[i].numpostings, cr[i].numcompressedwords, compressed/raw);
//        }
//    }

    
    /* find length of longest list */
    //******************************
//    int maxlength = 0;
//    for (i = 0; i < listnumber; i++) {
//        if (cr[i].listlength > maxlength) {
//            maxlength = cr[i].listlength;
//        }
//    }
//    printf("length of longest list: %d\n", maxlength);

    
     /* print list length frequencies */
    //***********************************
//    int * lengthfreqs = malloc(maxlength * sizeof lengthfreqs[0]);
//    memset(lengthfreqs, 0, maxlength * sizeof lengthfreqs[0]);
//    for (i = 0; i < numberoflists; i++) {
//        int temp = cr[i].numpostings;
//        lengthfreqs[temp]++;
//    }
//    for (i = 0; i < 60000; i++) {
//        printf("%d, %d\n", i, lengthfreqs[i]);
//    }


    /* cumulative selector frequencies for wsj postings_lists; */
    //*********************************************************
//    for (int j = 0; j < number_of_selectors; j++) {
//        printf("%d, %d\n", selectorfreqs[j].selector, selectorfreqs[j].frequency);
//    }


    free(postings_list);
    free(dgaps);
    free(compressed);
    free(decoded);
    free(cr);
    
    return EXIT_SUCCESS;
}
