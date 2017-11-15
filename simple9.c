#include<stdio.h>
#include<stdlib.h>
#include<strings.h>
#include<stdint.h>
#include "mylib.h"
#include "flexarray.h"

int compressedwords = 0;    //number of 32 bit compressed words
int numcompressedints = 0;  //keeps track of number of integers compressed
int numints = 0;            //keep track of number of integers while decompressing

struct flexarrayrec {
    uint64_t capacity;
    uint64_t itemcount;
    uint64_t *items;
};

flexarray flexarray_new() {
    flexarray result = emalloc(sizeof *result);
    result->capacity = 2;
    result->itemcount = 0;
    result->items = emalloc(result->capacity * sizeof result->items[0]);
    return result;
}

void fappend(flexarray f, uint64_t num) {
    if (f->itemcount == f->capacity) {
        f->capacity *= 2;
        f->items = erealloc(f->items, f->capacity * sizeof f->items[0]);
    }
    f->items[f->itemcount++] = num;
}

void flexarray_print(flexarray f) {
    int i;
    for (i = 0; i < f->itemcount; i++) {
        printf("%llu\n", f->items[i]);
    }
}

void print_binary(uint64_t num) {
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

uint64_t compress(uint64_t selector, int thisindex, uint64_t *dgaps) {
    //printf("compressing from index: %d\n", thisindex);
    uint64_t code, shiftedcode, result = 0;
    int numcodes = 0;
    do {
        result = result | selector;
        code = dgaps[numcompressedints];
        //printf("0x%16llX\n", code);
        //printf("shifted code: \n");
        shiftedcode = code << (4 + (numcodes * selector));
        //printf("0x%16llX\n", shiftedcode);
        result = result | shiftedcode;
        //printf("current state of compressed word: \n");
        //printf("0x%16llX\n", result);
        numcompressedints++;
        numcodes++;
    } while (((numcodes + 1) * selector) < 29);
    return result;
}

//uses global variable <numints> to keep track of filling decompressed array
void decompress(uint64_t word, flexarray decompressed, int numints) {
    int i;
    uint64_t selector, mask, payload, temp;
    selector = word & 0xf;
    mask = (1 << (selector)) - 1;
    payload = word >> 4;
    for (i = 0; i < (28/selector) ; i++) {
        temp = payload & mask;
        fappend(decompressed, temp);
        payload = payload >> selector;
    }
}

int compare_ints(const void *a, const void *b) {
    const int *ia = (const int *) a;
    const int *ib = (const int *) b;
    //return (*ia > *ib) - (*ia < *ib);
    return *ia < *ib ? -1 : *ia == *ib ? 0 : 1;
}


int countwastedbits(uint64_t word) {
  int i, external;
  uint64_t selector;
  selector = word & 0xf;
  if (selector == 5) {
    external = 3;
  } else if (selector == 9 || selector == 3) {
      external = 1;
    } else {
    external = 0;
  }
  
  // count internally wasted bits
  //
    return 2;
}

int main(void) {
    uint64_t selector;
    uint64_t arraysize = 500, i, prev = 0;
    uint64_t *docnums, *dgaps;
    uint64_t bits, maxbits, current, elements;
    uint64_t index = 0;
    int bitsused;
    
    flexarray compresseddgaps = flexarray_new();
    
    //flexarray f = flexarray_new();
    //
    //while (1 == scanf("%d", &item)) {
    //    fappend(f, item);
    //}
    //arraysize = getsize(f);
    //flexarray_sort(f);
    //flexarray_print(f);
    
    docnums = emalloc(arraysize * sizeof docnums[0]);
    for (i = 0; i < arraysize; i++) {
        docnums[i] = rand() % (arraysize * 3);
    }
    
    qsort(docnums, arraysize, sizeof docnums[0], compare_ints);
  
    dgaps = emalloc(arraysize * sizeof dgaps[0]);
    
    for (i = 0; i < arraysize; i++) {
        dgaps[i] = docnums[i] - prev;
        prev = docnums[i];
    }
    
    //    printf("d-gaps:\n");
    //    columns = 0;
    //    for (i = 0; i < arraysize; i++) {
    //        printf("%4llu ", dgaps[i]);
    //        columns++;
    //        if(columns == 7) {
    //            printf("\n");
    //            columns = 0;
    //        }
    //    }
    //    printf("\n");
    
    while (index < arraysize) {
        elements = 0;
        maxbits = 0;
        index = numcompressedints;
        
        bitsused = 0;
        while (bitsused < 29) {
            current = dgaps[index++];
            bits = fls(current);
            elements++;
            if (bits > maxbits) {
                maxbits = bits;
            }
            bitsused = maxbits * elements;
        }
        elements--;
        
        if (maxbits > 28) {
            return(EXIT_FAILURE);
        } else if (maxbits > 14) {
            selector = 28;
        } else if (maxbits > 9) {
            selector = 14;
        } else if (maxbits > 7) {
            selector = 9;
        } else if (maxbits > 5) {
            selector = 7;
        } else {
            selector = maxbits;
        }
       
        uint64_t temp = compress(selector, numcompressedints, dgaps);
        fappend(compresseddgaps, temp);
        compressedwords++;
    }
    
    printf("%llu integers were compressed into %d words\n", arraysize, compressedwords);
    
    //printf("first compressed word: \n");
    //print_binary(compresseddgaps->items[0]);
    //printf("0x%16llX\n", compresseddgaps->items[0]);
    
    flexarray decompressed = flexarray_new();
    
    numints = 0;
    for (i = 0; i < compressedwords; i++) {
        decompress(compresseddgaps->items[i], decompressed, numints);
    }

    int *wastedbits = malloc(compressedwords);
    
    for (i = 0; i < compressedwords; i++) {
      wastedbits[i] = countwastedbits(compresseddgaps->items[i]);
    }


    
    //    for (i = 0; i < arraysize; i++) {
    //        printf("original: %llu ", dgaps[i]);
    //        printf("decompressed: %llu ", decompressed->items[i]);
    //        if (dgaps[i] != decompressed->items[i]) {
    //            printf("wrong");
    //        };
    //        printf("\n");
    //    }

    //have forgotten to free items in flexarrays... needs fixing
    free(compresseddgaps);
    free(dgaps);
    free(docnums);
    
    return 0;
}
