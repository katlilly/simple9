#include<stdio.h>
#include<stdlib.h>
#include<strings.h>
#include<stdint.h>
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
  int i, prev = 0;
  int arraysize = 1000;
  int gap = 0, maxgap = 0;
  int *docnums, *dgaps;

  // make set of random numbers to use as document numbers
  docnums = malloc(arraysize * sizeof docnums[0]);
  for (i = 0; i < arraysize; i++) {
    docnums[i] = rand() % (arraysize * 500);
  }

  // sort them and convert to dgaps
  qsort(docnums, arraysize, sizeof docnums[0], compare_ints);
  dgaps = malloc(arraysize * sizeof dgaps[0]);
  for (i = 0; i < arraysize; i++) {
    dgaps[i] = docnums[i] - prev;
    prev = docnums[i];
  }

  // find dgap frequencies
  for (i = 0; i < arraysize; i++) {
    gap = dgaps[i];
    if (gap > maxgap) {
      maxgap = gap;
    }
  }
  printf("maxgap: %d\n", maxgap);
  
  bitlengthfreqs = malloc(20 * sizeof bitlengthfreqs[0]);
  for (i = 0; i < 20; i++) {
    bitlengthfreqs[i] = 0;
  }
  
  for (i = 0; i < arraysize; i++) {
    if (dgaps[i] < 2) {
      bitlengthfreqs[1]++;
    } else if (dgaps[1] < 4) {
      bitlegnthfreqs[2]++;
    } else if (dgaps[i] < 8) {
      bitlengthfreqs[3]++;
    } else if (dgaps[1] < 16) {
      bitlegnthfreqs[4]++;
    } else if (dgaps[i] < 32) {
      bitlengthfreqs[5]++;
    } else if (dgaps[1] < 64) {
      bitlegnthfreqs[6]++;
    } else if (dgaps[i] < 2) {
      bitlengthfreqs[7]++;
    } else if (dgaps[1] < 4) {
      bitlegnthfreqs[9]++;
    } else if (dgaps[i] < 8) {
      bitlengthfreqs[10]++;
    } else if (dgaps[1] < 16) {
      bitlegnthfreqs[11]++;
    } else if (dgaps[i] < 32) {
      bitlengthfreqs[]++;
    } else if (dgaps[12] < 64) {
      bitlegnthfreqs[6]++;
    } else if (dgaps[13] < 2) {
      bitlengthfreqs[7]++;
    } else if (dgaps[14] < 4) {
      bitlegnthfreqs[8]++;
    } else if 

  return 0;

}
