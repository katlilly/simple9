#include<stdio.h>
#include<stdlib.h>
#include<strings.h>
#include<stdint.h>
#include<math.h>
#include "flexarray.h"

// make synthetic dgap data

// find frequencies of each dgap

// find optimal set of selectors for a given set of dgaps

// compare compression ratio for optimised-selectors compression vs
// std-selectors compression


struct flexarrayrec {
    uint64_t capacity;
    uint64_t itemcount;
    uint64_t *items;
};

flexarray flexarray_new() {
    flexarray result = malloc(sizeof *result);
    result->capacity = 2;
    result->itemcount = 0;
    result->items = malloc(result->capacity * sizeof result->items[0]);
    return result;
}

void fappend(flexarray f, uint64_t num) {
    if (f->itemcount == f->capacity) {
        f->capacity *= 2;
        f->items = realloc(f->items, f->capacity * sizeof f->items[0]);
    }
    f->items[f->itemcount++] = num;
}

int compare_ints(const void *a, const void *b) {
    const int *ia = (const int *) a;
    const int *ib = (const int *) b;
    return *ia < *ib ? -1 : *ia == *ib ? 0 : 1;
}


int main(void) {
    int i, bits, sum, prev = 0;
    int freqarraysize, arraysize = 5000;
    int gap = 0, maxgap = 0;
    int *docnums, *dgaps;

    // make set of random numbers to use as document numbers
    docnums = malloc(arraysize * sizeof docnums[0]);
    for (i = 0; i < arraysize; i++) {
        docnums[i] = rand() % (arraysize * 100);
    }

    // sort them and convert to dgaps
    qsort(docnums, arraysize, sizeof docnums[0], compare_ints);
    dgaps = malloc(arraysize * sizeof dgaps[0]);
    for (i = 0; i < arraysize; i++) {
        dgaps[i] = docnums[i] - prev;
        prev = docnums[i];
    }

    // find size of max dgap before making array of bitlengths
    for (i = 0; i < arraysize; i++) {
        gap = dgaps[i];
        if (gap > maxgap) {
            maxgap = gap;
        }
    }
    printf("maxgap: %d\n", maxgap);
    freqarraysize = fls(maxgap) + 1;
  
    // array to hold bit length frequencies
    int *bitlengthfreqs = malloc(freqarraysize * sizeof bitlengthfreqs[0]);
    for (i = 0; i < freqarraysize; i++) {
        bitlengthfreqs[i] = 0;
    }

    /* // set frequencies 256 bit version */
    /* for (i = 0; i < arraysize; i++) { */
    /*     bits = fls(dgaps[i]); */
    /*     if (bits > 32) { */
    /*         return(EXIT_FAILURE); */
    /*     } else if (bits > 21) { */
    /*         bits = 32; */
    /*         bitlengthfreqs[bits]++; */
    /*     } else if (bits > 16) { */
    /*         bits = 21; */
    /*         bitlengthfreqs[bits]++; */
    /*     } else if (bits > 12) { */
    /*         bits = 16; */
    /*         bitlengthfreqs[bits]++; */
    /*     } else if (bits > 10) { */
    /*         bits = 12; */
    /*         bitlengthfreqs[bits]++; */
    /*     } else { */
    /*         bitlengthfreqs[bits]++; */
    /*     } */
    /* } */
    /* printf("14 valid bit lengths:\n"); */
    /* for (i = 0; i < freqarraysize; i++) { */
    /*     printf("number of dgaps requiring %d bits: %d\n", i, bitlengthfreqs[i]); */
    /* } */
    /* printf("\n"); */
    /* int sum = 0; */
    /* for (i = 0; i < freqarraysize; i++) { */
    /*     sum += bitlengthfreqs[i]; */
    /* } */
    /* printf("sum (should be 1000): %d\n", sum); */
  
    
    /* // set frequencies 128 bit version */
    /* for (i = 0; i < freqarraysize; i++) { */
    /*     bitlengthfreqs[i] = 0; */
    /* } */
    /* for (i = 0; i < arraysize; i++) { */
    /*     bits = fls(dgaps[i]); */
    /*     if (bits > 32) { */
    /*         return(EXIT_FAILURE); */
    /*     } else if (bits > 16) { */
    /*         bits = 32; */
    /*         bitlengthfreqs[bits]++; */
    /*     } else if (bits > 10) { */
    /*         bits = 16; */
    /*         bitlengthfreqs[bits]++; */
    /*     } else if (bits > 8) { */
    /*         bits = 10; */
    /*         bitlengthfreqs[bits]++; */
    /*     } else if (bits > 6) { */
    /*         bits = 8; */
    /*         bitlengthfreqs[bits]++; */
    /*     } else { */
    /*         bitlengthfreqs[bits]++; */
    /*     } */
    /* } */
    /* printf("10 valid bit lengths:\n"); */
    /* for (i = 0; i < freqarraysize; i++) { */
    /*     printf("number of dgaps requiring %d bits: %d\n", i, bitlengthfreqs[i]); */
    /* } */
    /* printf("\n"); */
    /* sum = 0; */
    /* for (i = 0; i < freqarraysize; i++) { */
    /*     sum += bitlengthfreqs[i]; */
    /* } */
    /* printf("sum (should be 1000): %d\n", sum); */


    //set frequencies
    for (i = 0; i < freqarraysize; i++) {
        bitlengthfreqs[i] = 0;
    }
    for (i = 0; i < arraysize; i++) {
        bits = fls(dgaps[i]);
        if (bits < 2) { //won't be zeros in real data, ones should be run length encoded
            bits = 1;
            bitlengthfreqs[bits]++;
        } else if (bits < 17) {
            bitlengthfreqs[bits]++;
        } else if (bits < 32) {
            bits = 16;
            bitlengthfreqs[bits]++;
        } else if (bits == 32) {
            bitlengthfreqs[bits]++;
        } else {
            return(EXIT_FAILURE);
        }
    }
  

    printf("unrestricted bit lengths:\n");
    for (i = 0; i < freqarraysize; i++) {
        printf("number of dgaps requiring %d bits: %d\n", i, bitlengthfreqs[i]);
    }
    sum = 0;
    for (i = 0; i < freqarraysize; i++) {
        sum += bitlengthfreqs[i];
    }
    printf("sum (should be 1000): %d\n", sum);
    
    return 0;

}
