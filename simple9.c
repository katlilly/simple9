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
// cumulative length of all lists in wsj dataset is 41 205 930
// so longest list is 0.4% of total data

/* data structure for each line in the selector table */
typedef struct
{
    uint32_t bits;
    int intstopack;     // number of ints to pack
    uint32_t masks;
} selector;


/* data structure for selector-use statistics */
typedef struct
{
    int selector;
    int frequency;
} stats;


/* data structure for measuring internally wasted bits for each selector */
typedef struct
{
    int selector;
    int timesused;
    int wastedbits;
} wastedbits;


// probably this should be declared in main, not here?
wastedbits wb[] =
{
    {1,  0, 0},
    {2,  0, 0},
    {3,  0, 0},
    {4,  0, 0},
    {5,  0, 0},
    {7,  0, 0},
    {9,  0, 0},
    {14, 0, 0},
    {28, 0, 0}
};

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
    int topack;                                 // min of intstopack and what's available to compress
    uint32_t *integer = raw;                    // the current integer to compress
    uint32_t *end = raw + integers_to_compress; // the end of the input array
    
    /* chose selector */
    for (which = 0; which < number_of_selectors; which++)
    {
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
    
    /* below is the way andrew wrote it, which packs ints in reverse order relative to my decompress */
//    *destination = 0; // initialize word to zero before packing ints and selector into it
//    for (current = 0; current < table[which].intstopack; current++) {
//        uint32_t value = current > integers_to_compress ? 0 : raw[current];
//        // value of current int (or pack some zeros in last word if we're at end of list)
//        *destination = *destination << table[which].bits | value; // pack ints into compressed word
//        // print_binary(*destination);
//    }
//    *destination = *destination << 4 | which; // put the selector into compressed word
    
    return topack;    // return number of dgaps compressed into this word
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
        decoded[intsout + offset] = temp;
        intsout++;
        payload = payload >> table[selector].bits;
    }
    return intsout;
}

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
        wb[selector].wastedbits += wasted;
        decoded[intsout + offset] = temp;
        intsout++;
        payload = payload >> table[selector].bits;
    }
    wb[selector].timesused++;
    return intsout;
}


void print_wasted_bits_per_selector_human(wastedbits *wbs)
{
    printf("selector:  times used:  wastedbits:  wasted per word:\n");
    for (int i = 0; i < number_of_selectors; i++) {
        printf("%2d, \t\t%5d, \t\t\t%5d", table[i].bits, wbs[i].timesused, wbs[i].wastedbits);
        printf("\t\t %.2f\n", (double) wbs[i].wastedbits / wbs[i].timesused);
    }
}

void print_wasted_bits_per_selector_csv(wastedbits *wbs)
{
    for (int i = 0; i < number_of_selectors; i++) {
        printf("%d, %d, %d, ", table[i].bits, wbs[i].timesused, wbs[i].wastedbits);
        printf("%.2f\n", (double) wbs[i].wastedbits / wbs[i].timesused);
    }
}



int compare_ints(const void *a, const void *b) {
    const int *ia = (const int *) a;
    const int *ib = (const int *) b;
    return *ia < *ib ? -1 : *ia == *ib ? 0 : 1;
}




/* function for making synthetic data that will compress in a known way */
uint32_t * makefakedata(uint32_t *dest, int number, int numberofnumbers) {
    int i;
    for (i = 0; i < numberofnumbers; i++) {
        dest[i] = number;
    }
    return dest;
}


/* measure bit differences in uncompressed list
   return a frequency array of how many times an int is x bits
   different from previous int */
int * count_bitdiffs(uint32_t *raw, int length)
{
    int diff;
    int *bitdiffs = malloc(32 * sizeof *bitdiffs);
    memset(bitdiffs, 0, 32 * sizeof *bitdiffs);
    for (int i = 5; i < length; i++) {
        if (fls(raw[i]) > fls(raw[i-5])) {
            diff = fls(raw[i]) - fls(raw[i-5]);
        } else {
            diff = fls(raw[i-5]) - fls(raw[i]);
        }
        bitdiffs[diff]++;
    }
    return bitdiffs;
}


/* measure bit length differences from neighbours, and output
   an array of bit lenth differences
   currently am setting all differences as positive numbers
   may want to change this in future?
   data returned from this function to be statistically analysed */
int * list_bitdiffs(uint32_t *raw, int length)
{
    int diff;
    int *bitdiffslist = malloc(length * sizeof *bitdiffslist);
    memset(bitdiffslist, 0, length * sizeof *bitdiffslist);
    for (int i = 1; i < length; i++) {
        if (fls(raw[i]) > fls(raw[i-1])) {
            diff = fls(raw[i]) - fls(raw[i-1]);
        } else {
            diff = fls(raw[i-1]) - fls(raw[i]);
        }
        bitdiffslist[i] = diff;
    }
    return bitdiffslist;
}


void print_bitdiffs(int *diffs)
{
    printf("diff: freq:\n");
    for (int i = 0; i < 10; i++) {
        printf("%d\t%d\n", i, diffs[i]);
    }
}



/* count selector used vs bitwidths for every int in a compressed list
   length is the compressed list length
   returns an array of frequencies for each selector / bitwidth pair */
int * count_bitsvselector(uint32_t *compressed, int length)
{
    int temp = 0;
    int max_bitwidth = 28;
    int number_of_selectors = 9;
    int *result = malloc(max_bitwidth * number_of_selectors * sizeof *result);
    memset(result, 0, max_bitwidth * number_of_selectors * sizeof *result);
    
    for (int i = 0; i < length; i++) {
        uint32_t word = *(compressed + i);
        int selector = word & 0xf;
        if (selector == 3) {
            //printf("this word compressed with %d bit selector\n", table[selector].bits);
            uint32_t payload = word >> 4;
            for (int j = 0; j < table[selector].intstopack; j++) {
                temp = payload & table[selector].masks;
                result[fls(temp) + selector * max_bitwidth]++;
                payload = payload >> table[selector].bits;
                //printf("%d, ", fls(temp));
            }
            //printf("\n");
        }
    }
    return result;
}



/* print bitwidths vs selector used for one compressed list in human readable form */
void print_bitsvselector_human(int *bitsvselector)
{
    int max_bitwidth = 28;
    int number_of_selectors = 9;
    for (int i = 0; i < max_bitwidth; i++) {
        printf("where the %d bit ints went:\n", i);
        printf("selector: number of times used by %d bit ints:\n", i);
        for (int j = 0; j < number_of_selectors; j++) {
            printf("  %2d:      %d\n", table[j].bits, bitsvselector[i + j * max_bitwidth]);
        }
        printf("\n");
    }
}

/* print selector used vs bitwidths for one compressed list in human readable form */
void print_selectorvbits_human(int *bitsvselector)
{
    int max_bitwidth = 28;
    int number_of_selectors = 9;
    for (int i = 0; i < number_of_selectors; i++) {
        printf("how the %d bit selector was used:\n", table[i].bits);
        printf("bit width: number of ints in selector\n");
        for (int j = 0; j < max_bitwidth; j++) {
            printf("   %2d:      %d\n", j, bitsvselector[j + i * max_bitwidth]);
        }
        printf("\n");
    }
}

/* print bitwidths vs selector used for one compressed list in csv form */
void print_bitsvselector_csv(int *bitsvselector)
{
    int max_bitwidth = 28;
    int number_of_selectors = 9;
    for (int i = 0; i < max_bitwidth; i++) {
        for (int j = 0; j < number_of_selectors; j++) {
            printf("%d, %d\n", table[j].bits, bitsvselector[i + j * max_bitwidth]);
        }
    }
}

/* print selector used vs bitwidths for one compressed list in csv form */
void print_selectorvbits_csv(int *bitsvselector)
{
    int max_bitwidth = 28;
    int number_of_selectors = 9;
    for (int i = 0; i < number_of_selectors; i++) {
            for (int j = 0; j < max_bitwidth; j++) {
            printf("%d, %d\n", j, bitsvselector[j + i * max_bitwidth]);
        }
    }
}

void print_list(uint32_t *list, int length)
{
    int i;
    for (i = 0; i < length; i++) {
        printf("%d\n", list[i]);
    }
    printf("\n");
}


int main(int argc, char *argv[])
{
    int listnumber = 0;
    uint32_t i, prev, length;
    uint32_t compressedwords;
    uint32_t compressedints;
    //long long cumulativelength = 0;
    long long cumulativelengths[5];
    
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
    
    for (i = 0; i < 5; i++) {
        cumulativelengths[i] = 0;
    }
    
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
        
        
        /* calculate what portion of the data is in lists of a given length */
        // ****************************************************************
//        if (length > UINT32_MAX - cumulativelength) {
//            printf("overflow wrap around about to happen\n");
//            exit(3);
//        } else {
//            cumulativelength += length;
//        }
//        //printf("length of current list: %d", l);
//        if (length < 100) {
//            cumulativelengths[0] += length;
//        } else if (length < 1000) {
//            cumulativelengths[1] += length;
//        } else if (length < 10000) {
//            cumulativelengths[2] += length;
//        } else if (length < 100000) {
//            cumulativelengths[3] += length;
//        } else {
//            cumulativelengths[4] += length;
//        }
        
        


        
        /* convert postings list to dgaps list */
        prev = 0;
        for (i = 0; i < length; i++) {
            dgaps[i] = postings_list[i] - prev;
            prev = postings_list[i];
        }
        
        
        
        /* an array for storing bit width statistics for a single list */
//        int *single_list_bitwidths = malloc(MAX_BITWIDTH * sizeof *single_list_bitwidths);
//        for (i = 0; i < MAX_BITWIDTH; i++) {
//            single_list_bitwidths[i] = 0;
//        }
        
        /* count bits needed for each dgap, both for a single list and
         as cumulative stats for entire set of lists */
//        int bitwidth;
//        for (i = 0; i < length; i++) {
//            bitwidth = fls(dgaps[i]);
//            bitwidths[bitwidth]++;
//            //single_list_bitwidths[bitwidth]++;
//        }
        
        
        
        
        /* print statistics for a specified single list */
        // *******************************************
        //using lists 96 and 445139 as examples
      
        if (listnumber == 96) {
            //printf("length of list %d is %d\n", listnumber, length);
            printf("%d\n", listnumber);
            printf("%d\n", length);
            
            print_list(postings_list, length);
            print_list(dgaps, length);
            
          
            /* get bit width data for a single list */
            // ************************************
//            int bitwidth;
//            for (i = 0; i < length; i++) {
//                bitwidth = fls(dgaps[i]);
//                //bitwidths[bitwidth]++;
//                single_list_bitwidths[bitwidth]++;
//            }
//            //printf("Bitwidth stats for %dth list: \n", listnumber);
//            for (i = 0; i < MAX_BITWIDTH; i++) {
//                printf("%d, %d\n", i, single_list_bitwidths[i]);
//            }
//            free(single_list_bitwidths);
            
            
            /* compress 1 list */
            uint32_t numencoded = 0;  // return value of encode function, the number of ints compressed
            compressedwords = 0;      // offset for position in output array "compressed"
            compressedints = 0;       // offset for position in input array "dgaps"
            for (compressedints = 0; compressedints < length; compressedints += numencoded) {
                numencoded = encode(compressed + compressedwords, dgaps + compressedints, length - compressedints);
                compressedwords++;
            }
            printf("compressed length of list %d: %d\n", listnumber, compressedwords);
            
            
            /* decompress a single list while counting selector use vs bitwidths */
            // *************************************
            int offset = 0;  // reset number of decompressed ints to zero for each word
            // don't look at last word because it likely isn't full
            for (i = 0; i < compressedwords - 1; i++) {
                offset += decompress_countwasted(decoded, compressed[i], offset);
            }
            
            int *list96bitdiffs = count_bitdiffs(dgaps, length);
            print_bitdiffs(list96bitdiffs);
            
            int *list96bitdiffslist = list_bitdiffs(dgaps, length);
            
            // measure variance of list of bitwidth differences from neighbours
            
            
        }// end specified single list stuff


        
        /* compress this postings list */
//        uint32_t numencoded = 0;  // return value of encode function, the number of ints compressed
//        compressedwords = 0;      // offset for position in output array "compressed"
//        compressedints = 0;       // offset for position in input array "dgaps"
//        for (compressedints = 0; compressedints < length; compressedints += numencoded) {
//            numencoded = encode(compressed + compressedwords, dgaps + compressedints, length - compressedints);
//            compressedwords++;
//        }
        
       
        
        /* add selector freqs for this compressed list to global array of selector freqs */
        // *****************************************************************************
//        for (j = 0; j < compressedwords; j++) {
//            int selector = compressed[j] & 0xf;
//            //if (compressedints > 100) { // only look at 'long' lists
//                selectorfreqs[selector].frequency += 1;
//            //}
//            //printf("selector %d\n", table[selector].bits);
//        }

        
        /* add compression ratio stats for this list */
        // *****************************************
//        cr[listnumber].listlength = length;
//        cr[listnumber].numcompressedwords = compressedwords;

        /* count wasted bits in entire compressed list */
        // ****** not right *********
//        wastedbits = 0;
//        // don't look at last word because it might not be full
//        if (length > 100) {
//            for (i = 0; i < compressedwords - 1; i++) {
//                //print_binary(compressed[i]);
//                wastedbits += countwastedbits(compressed[i]);
//                //qprintf("wasted bits in %dth word: %d\n", i, wastedbits);
//            }
//        }
        // then add total internally wasted bits for this list to cr[listnumber].wastedbits

        /* decompress the current postings list */
        // ************************************
//        int offset = 0;  // reset number of decompressed ints to zero for each word
//        for (i = 0; i < compressedwords; i++) {
//            offset += decompress(decoded, compressed[i], offset);
//        }

        
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

    /* print list length data */
    // **********************
//    printf("cumulative length of all lists: %lld\n", cumulativelength);
//    // the answer is 41205930
//    for (i = 0; i < 5; i++) {
//        printf("%lld\n", cumulativelengths[i]);
//    }
    
    
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
    
    return 0;
}
