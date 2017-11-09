#include<stdio.h>
#include<stdlib.h>
#include<strings.h>
#include "mylib.h"
#include "flexarray.h"

int compressedwords = 0;
int numcompressedints = 0;

struct flexarrayrec {
    uint64_t capacity;
    uint64_t itemcount;
    uint64_t *items;
};

int getsize(flexarray f) {
    return f->itemcount;
}

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


void print_bigendian(uint64_t num) {
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
    printf("current selector: %llu\n", selector);
    print_bigendian(selector);
    
    //index = index - (28/selector);
    printf("compressing from index: %d\n", thisindex);
    uint64_t code, result = 0;
    int numcodes = 0;
    do {
        result += selector;
        printf("%dth code: \n", numcodes);
        code = dgaps[numcompressedints];
        print_bigendian(code);
        printf("shifted code: \n");
        code = code << (4 + numcodes * selector);
        print_bigendian(code);
        result = (result | code);
        printf("current state of compressed word: \n");
        print_bigendian(result);
        numcompressedints++;
        numcodes++;
    } while (((numcodes+1) * selector) < 29);
    //printf("compressing with selector %c\n", selector);
    //compressedwords++;
    
    return result;
}

int compare_ints(const void *a, const void *b) {
    const int *ia = (const int *) a;
    const int *ib = (const int *) b;
    //return (*ia > *ib) - (*ia < *ib);
    return *ia < *ib ? -1 : *ia == *ib ? 0 : 1;
}


int main(void) {
    uint64_t selector;
    uint64_t arraysize = 500, i, prev = 0;
    uint64_t *docnums, *dgaps;
    uint64_t bits, maxbits, current, elements;
    uint64_t index = 0;
    int columns;
    
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
        docnums[i] = rand() % (arraysize *3);
        //printf("%llu, ", docnums[i]);
    }
    
    qsort(docnums, arraysize, sizeof docnums[0], compare_ints);
  
    dgaps = emalloc(arraysize * sizeof dgaps[0]);
    
    for (i = 0; i < arraysize; i++) {
        dgaps[i] = docnums[i] - prev;
        prev = docnums[i];
    }
    
    printf("d-gaps:\n");
    
    columns = 0;
    for (i = 0; i < arraysize; i++) {
        printf("%4llu ", dgaps[i]);
        columns++;
        if(columns == 10) {
            printf("\n");
            columns = 0;
        }
    }
    
    while (index < arraysize) {
        elements = 0;
        maxbits = 0;
        index = numcompressedints;

        // this doesn't work right, in some cases we compress with
        // more bits than needed because the following element needs it.
        // does a valid encoding, but with more sometimes bits than necessary
        while ((maxbits * elements) < 29){
            current = dgaps[index++];
            elements++;
            bits = fls(current);
            printf("%llu bits, ", bits);
            if (bits > maxbits) {
                maxbits = bits;
            }
        } //while (maxbits * elements < 29);
        
        
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
        printf("compressing %dth word with selector %llu\n", compressedwords, selector);
        printf("current index is: %llu\n", index);
        int temp = compress(selector, numcompressedints, dgaps);
        fappend(compresseddgaps, temp);
        compressedwords++;
    }
    
    //compress(3, 35, dgaps);
    flexarray_print(compresseddgaps);
    printf("number of integers compressed: %llu\n", arraysize);
    printf("compressed into %d words\n", compressedwords);
    
    printf("first compressed word in binary: \n");
    print_bigendian(compresseddgaps->items[0]);
    
    free(compresseddgaps);
    free(dgaps);
    free(docnums);
    
    return 0;
}
