#include<stdio.h>
#include<stdlib.h>
#include<strings.h>
#include<stdint.h>
#include "mylib.h"
#include "flexarray.h"
//#include "fls.h"

#define NUMBER_OF_DOCS (1024 * 1024 * 128)

static uint32_t *postings_list;
uint32_t *dgaps, *compressed, *decompressed;


typedef struct
{
    uint32_t bits;
    int numbers;
    uint32_t masks;
} selector;


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


typedef struct
{
    int selector;
    int frequency;
} stats;


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


// stuct for recording statistics for a single postings list
typedef struct
{
    int numpostings;
    int numcompressedwords;
    int wastedbits;
} ratios;

// counted number of postings in wsj dataset in main method = 499692
ratios cr[499692];


int number_of_selectors = sizeof(table) / sizeof(*table);


void print_binary(uint32_t num) {
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


uint32_t encode(uint32_t *destination, uint32_t *raw, uint32_t integers_to_compress)
{
    uint32_t which;                             // which element in selector struct array
    int current;                                // count of elements within each compressed word
    uint32_t *integer = raw;                    // the current integer to compress
    uint32_t *end = raw + integers_to_compress; // the end of the input array
    for (which = 0; which < number_of_selectors; which++)
    { // start by assuming we can fit 28 ints in
        // calculate end point - either end of list or enough ints for a word
        end = raw + min(integers_to_compress, table[which].numbers);
        for (; integer < end; integer++)
        {
            if (fls(*integer) > table[which].bits)
                break; // increment which if current integer can't fit this many bits
        }
        if (integer >= end) {
            break;
        }
    }
    //printf("chose selector: %d, will encode %d ints\n", table[which].bits, table[which].numbers);
    *destination = 0; // initialize word to zero before packing ints and selector into it
    for (current = 0; current < table[which].numbers; current++)
    {
        uint32_t value = current > integers_to_compress ? 0 : raw[current];
        // value of current int (or pack some zeros in last word if we're at end of list)
        *destination = *destination << table[which].bits | value; // pack ints into compressed word
    }
    *destination = *destination << 4 | which; // put the selector into compressed word
    // printf("compressed %d ints\n", end - raw);
    // return end - raw;    // return number of dgaps compressed so far.
    // below line keeps control of where we're up to properly, but does not return correct value
    // for the last compressed word
    return table[which].numbers;
}


// uses global variable <numints> to keep track of filling decompressed array
void decompress(uint32_t word, uint32_t *decompressed, int numints) {
    int i;
    uint32_t selector, mask, payload, temp;
    selector = word & 0xf;
    //selectorfreqs[selector]++;
    mask = (1 << (selector)) - 1;
    payload = word >> 4;
    for (i = 0; i < (28/selector) ; i++) {
        temp = payload & mask;
        decompressed[numints++] = temp;
        payload = payload >> selector;
    }
}


uint32_t decode(uint32_t *destination, uint32_t word) {
    uint32_t selector = word & 0xf;
    int bits = 0;
    for (int i = 0; i < number_of_selectors; i++)
    {
        if (selector == table[i].bits)
        {
            bits = table[i].bits;
        }
    }
    uint32_t numdecompressed = table[bits].numbers;
    uint32_t payload = word >> 4;
    for (int i = 0; i < table[selector].numbers; i++) {
        
        
    }
    
    return numdecompressed;
}


int compare_ints(const void *a, const void *b) {
    const int *ia = (const int *) a;
    const int *ib = (const int *) b;
    return *ia < *ib ? -1 : *ia == *ib ? 0 : 1;
}


// this function repeats most of what the decompression function does
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


int getstats() {
    
    return 1;
    
}


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
    //int totalwastedbits, totalcompresedwords;
    uint32_t i, j, prev = 0;
    uint32_t compressedwords;
    uint32_t compressedints;
    
    const char *filename;
    if (argc == 2) {
        filename = argv[1];
    } else {
        exit(printf("Usage::%s <binfile>\n", argv[0]));
    }
    
    postings_list = malloc(NUMBER_OF_DOCS * sizeof postings_list[0]);
    dgaps = malloc(NUMBER_OF_DOCS * sizeof dgaps[0]);
    compressed = malloc(NUMBER_OF_DOCS * sizeof compressed[0]);
    decompressed = malloc(NUMBER_OF_DOCS * sizeof decompressed[0]);
    
    //printf("Using: %s\n", filename);
    
    FILE *fp;
    if ((fp = fopen(filename, "rb")) == NULL) {
        exit(printf("Cannot open %s\n", filename));
    }
    
    uint32_t length;
    while (fread(&length, sizeof(length), 1, fp)  == 1) {
        /*
         Read one postings list (and make sure we did so successfully)
         */
        if (fread(postings_list, sizeof(*postings_list), length, fp) != length) {
            exit(printf("i/o error\n"));
        }
        
        //print current postings list
        //printf("%u: ", (unsigned)length);
        for (uint32_t *where = postings_list; where < postings_list + length; where++) {
            //printf("%u ", (unsigned)*where);
        }
        //printf("\n");
        
        // convert postings list to dgaps list
        prev = 0;
        for (i = 0; i < length; i++) {
            dgaps[i] = postings_list[i] - prev;
            prev = postings_list[i];
        }
        
        // compress postings list
        uint32_t encoded = 0;  // return value of encode function - number of ints compressed
        compressedwords = 0;   // position in output array
        compressedints = 0;    // position in input array
        
        for (compressedints = 0; compressedints < length; compressedints += encoded)
        {
            encoded = encode(compressed + compressedwords, dgaps + compressedints, length - compressedints);
            compressedwords++;
        }
        //printf("number of ints compressed: %d\n", compressedints);
        //printf("mumber of compressed words: %d\n", compressedwords);
        
        // add selector frequencies for each compressed list to global selector freqs array
        for (j = 0; j < compressedwords; j++) {
            //printf("%0x\n", compressed[j]);
            int selector = compressed[j] & 0xf;
            if (compressedints > 100) {
                selectorfreqs[selector].frequency += 1;
            }
            //printf("selector %d\n", table[selector].bits);
        }
        
        listnumber++;
        
        cr[listnumber].numpostings = compressedints;
        cr[listnumber].numcompressedwords = compressedwords;
        
        //count wasted bits in entire compressed list
        wastedbits = 0;
        for (i = 0; i < compressedwords; i++) {
            wastedbits += countwastedbits(compressed[i]);
        }
        
        
        printf("wasted bits in %dth list: %d\n", listnumber, wastedbits);
        double dwasted = (double) wastedbits;
        double dwords = (double) compressedwords;
        double meanwasted = dwasted/dwords;
        printf("compressed length: %d, wasted bits per word: %d\n", compressedwords, meanwasted);

        
    }// end read-in of postings_list
    
    
    
    int numberoflists = listnumber;
    //printf("number of lists: %d\n", numberoflists);
    
    
    // compression ratios
    for (i = 0; i < numberoflists; i++) {
        if (cr[i].numpostings > 100) {
            double compressed = (double) cr[i].numcompressedwords;
            double raw = (double) cr[i].numpostings;
            //printf("list number: %7d, list length: %2d, compressed length: %2d, compression ratio: %f\n", i, cr[i].numpostings, cr[i].numcompressedwords, compressed/raw);
        }
    }
    
    // list length statistics
    int maxlength = 0;
    for (i = 0; i < numberoflists; i++) {
        if (cr[i].numpostings > maxlength) {
            maxlength = cr[i].numpostings;
        }
    }
    
    int * lengthfreqs = malloc(maxlength * sizeof lengthfreqs[0]);
    memset(lengthfreqs, 0, maxlength * sizeof lengthfreqs[0]);
    
    for (i = 0; i < numberoflists; i++) {
        int temp = cr[i].numpostings;
        lengthfreqs[temp]++;
    }
    
    for (i = 0; i < maxlength; i++) {
        //printf("%d, %d\n", i, lengthfreqs[i]);
    }
    
    
    // cumulative selector frequencies for wsj postings_lists;
    for (int j = 0; j < number_of_selectors; j++) {
        printf("selector: %2d, frequency: \t%8d\n", selectorfreqs[j].selector, selectorfreqs[j].frequency);
    }
    
    
    
    
    
   
//    printf("second compressed word: \n");
//    print_binary(compressed[1]);
//    printf("0x%8X\n", compressed[1]);
//    for (i = 0; i < compressedwords; i++) {
//        printf("%0x\n", compressed[i]);
//        printf("dgaps:  compressed:  decompressed\n");
//        printf(" %d      %0x       %d\n", dgaps[i], compressed[i], decompressed[i]);
//        printf("\n");
//    }
    
    free(postings_list);
    free(dgaps);
    free(compressed);
    //free(decompressed);
    
    return EXIT_SUCCESS;
}
