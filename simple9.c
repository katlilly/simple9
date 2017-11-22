#include<stdio.h>
#include<stdlib.h>
//#include<strings.h>
#include<stdint.h>
#include "mylib.h"
#include "flexarray.h"
#include "fls.h"

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

// currently sort of works, but does not always chose the right selector
// doesn't increment integer pointer the way i think it should
uint32_t encode(uint32_t *destination, uint32_t *raw, uint32_t integers_to_compress)
{
    uint32_t which;                             // which line in selector table
    int current;                                // element number within each word
    uint32_t *integer = raw;                    // the current integer to compress
    uint32_t *end = raw + integers_to_compress; // the end of the input array
    for (which = 0; which < number_of_selectors; which++)
    { // start by assuming we can fit 28 ints in
        // calculate end point - either end of list or enough ints for a word
        end = raw + min(integers_to_compress, table[which].numbers);
        for (; integer < end; integer++)
        {
            printf("current integer: %d, fls(integer): %d\n", *integer, fls(*integer));
            if (fls(*integer) > table[which].bits)
                break;
        }
        if (integer >= end)
            break;
        else
            if (integer == end)
                which++;
    }
    printf("chose selector: %d, will encode %d ints\n", table[which].bits, table[which].numbers);
    *destination = 0; // initialize word to zero before packing ints and selector into it
    for (current = 0; current < table[which].numbers; current++)
    {
        uint32_t value = current > integers_to_compress ? 0 : raw[current];
        *destination = *destination << table[which].bits | value; // pack ints into compressed word
    }
    *destination = *destination << 4 | table[which].bits; // put the selector into compressed word
    printf("%0x\n", *destination);
    print_binary(*destination);
    //printf("compressed %d ints\n", end - raw);
    //return end - raw;    // return number of dgaps compressed so far.
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

uint32_t * makefakedata(uint32_t *dest, int number, int numberofnumbers) {
    int i;
    for (i = 0; i < numberofnumbers; i++) {
        dest[i] = number;
    }
    return dest;
}

int main(int argc, char *argv[]) {
    uint32_t i, prev = 0;
    
    //const char *filename;
    //    if (argc == 2) {
    //        filename = argv[1];
    //    } else {
    //        exit(printf("Usage::%s <binfile>\n", argv[0]));
    //    }
    
    postings_list = malloc(NUMBER_OF_DOCS * sizeof postings_list[0]);
    dgaps = malloc(NUMBER_OF_DOCS * sizeof dgaps[0]);
    compressed = malloc(NUMBER_OF_DOCS * sizeof compressed[0]);
    decompressed = malloc(NUMBER_OF_DOCS * sizeof decompressed[0]);
    
    //printf("Using: %s\n", filename);
    //    reading in of real data commented out until i get compress/decompress working again
    //    FILE *fp;
    //    if ((fp = fopen(filename, "rb")) == NULL) {
    //        exit(printf("Cannot open %s\n", filename));
    //    }
    //
    //    uint32_t length;
    //    while (fread(&length, sizeof(length), 1, fp)  == 1) {
    //        /*
    //         Read one postings list (and make sure we did so successfully)
    //         */
    //        // write one postings list into the postings_list array
    //        if (fread(postings_list, sizeof(*postings_list), length, fp) != length) {
    //            exit(printf("i/o error\n"));
    //        }
    //
    // print current postings list
    //        printf("%u: ", (unsigned)length);
    //        for (uint32_t *where = postings_list; where < postings_list + length; where++) {
    //            printf("%u ", (unsigned)*where);
    //        }
    //        printf("\n");
    //
    //        //convert postings list to dgaps list
    //        prev = 0;
    //        for (i = 0; i < length; i++) {
    //            dgaps[i] = postings_list[i] - prev;
    //            prev = postings_list[i];
    //        }
    
    // make some fake data for testing
//    int length = 100;
//    for (i = 0; i < 28; i++)
//    {
//        postings_list[i] = 1;
//    }
//
//    qsort(postings_list, length, sizeof postings_list[0], compare_ints);
//    prev = 0;
//    for (i = 0; i < length; i++) {
//        dgaps[i] = postings_list[i] - prev;
//        prev = postings_list[i];
//        if (dgaps[i] == 0) {
//            dgaps[i] = 1;
//        }
//    }
//    for (i = 0; i < length; i++) {
//        printf("%d, ", dgaps[i]);
//    }
//    printf("\n");
    // end fake data
    
    
    // check each selector
    int length = 28;
    for (i = 0; i < number_of_selectors; i++)
    {
        int number = table[i].masks;
        dgaps = makefakedata(dgaps, number, 28);
        uint32_t encoded = 0;           // return value of encode function - num ints compressed
        uint32_t compressedwords = 0;   // position in output array
        uint32_t compressedints = 0;    // position in input array
        for (compressedints = 0; compressedints < length; compressedints += encoded)
        {
            encoded = encode(compressed + compressedwords, dgaps + compressedints, length - compressedints);
            compressedwords++;
        }
    }
    //dgaps = makefakedata(dgaps, 1, 28);
    //dgaps = makefakedata(dgaps, 2, 14);
    //dgaps = makefakedata(dgaps, 3, 9);
    
    
    
    
    // decompress
    uint32_t decompressedwords = 0;
    uint32_t decompressedints = 0;
    
    
    //} // end while loop for reading in postings lists
    
    //printf("second compressed word: \n");
    //print_binary(compressed[1]);
    //printf("0x%8X\n", compressed[1]);
         // for (i = 0; i < compressedwords; i++) {
         //     printf("%0x\n", compressed[i]);
//              printf("dgaps:  compressed:  decompressed\n");
  //            printf(" %d      %0x       %d\n", dgaps[i], compressed[i], decompressed[i]);
    //          printf("\n");
       //   }
    
    free(postings_list);
    free(dgaps);
    free(compressed);
    free(decompressed);
    
    return 0;
}
