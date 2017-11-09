#include<stdio.h>
#include<stdlib.h>
#include<strings.h>
#include "mylib.h"
#include "flexarray.h"

int compressedwords = 0;

struct flexarrayrec {
    int capacity;
    int itemcount;
    int *items;
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

void fappend(flexarray f, int num) {
    if (f->itemcount == f->capacity) {
        f->capacity *= 2;
        f->items = erealloc(f->items, f->capacity * sizeof f[0]);
    }
    f->items[f->itemcount++] = num;
}

void flexarray_print(flexarray f) {
    int i;
    for (i = 0; i < f->itemcount; i++) {
        printf("%d\n", f->items[i]);
    }
}


void flexarray_sort(flexarray f) {
    int p, key;
    for (p = 1; p < f->itemcount; p++) {
        key = f->items[p];
        while (f->items[p-1] > key && p > 0) {
            f->items[p] = f->items[p-1];
            p--;
        }
        f->items[p] = key;
    }
}


void flexarray_free(flexarray f) {
    free(f);
}

void print_bigendian(int num) {
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


//just case c for now, three-bit codes
unsigned int compress(unsigned int selector, int index, unsigned int *dgaps) {
    printf("current selector: %d\n", selector);
    print_bigendian(selector);
    //selector = 3;
    index = index - (28/selector);
    unsigned int code, result = 0;
    int numcodes = 0;
    do {
        result += selector;
        printf("%dth code: \n", numcodes);
        code = dgaps[index];
        print_bigendian(code);
        printf("shifted code: \n");
        code = code << (4 + numcodes * selector);
        print_bigendian(code);
        result = (result | code);
        printf("current state of compressed word: \n");
        print_bigendian(result);
        index++;
        numcodes++;
    } while (((numcodes+1) * selector) < 29);
    //printf("compressing with selector %c\n", selector);
    //compressedwords++;
    
    return result;
}




int main(void) {
    unsigned int selector;
    unsigned int item, arraysize, i, prev = 0;
    unsigned int *dgaps;
    unsigned int bits, maxbits, current, elements;
    int index = 0;
    
    
    flexarray f = flexarray_new();
    flexarray compressed = flexarray_new();
    
    while (1 == scanf("%d", &item)) {
        fappend(f, item);
    }
    
    arraysize = getsize(f);
    flexarray_sort(f);
   // flexarray_print(f);
    
    dgaps = emalloc(arraysize * sizeof dgaps[0]);
    
    for (i = 0; i < arraysize; i++) {
        dgaps[i] = 0;
    }
    
    for (i = 0; i < arraysize; i++) {
        dgaps[i] = f->items[i] - prev;
        prev = f->items[i];
    }
    
    //printf("d-gaps:\n");
    //for (i = 0; i < arraysize; i++) {
    //    printf("%d\n", dgaps[i]);
    //}
    
    while (index < arraysize) {
        elements = 0;
        maxbits = 0;

        do {
            current = dgaps[index];
            index++;
            elements++;
            bits = fls(current);
            if (bits > maxbits) {
                maxbits = bits;
            }
        } while (maxbits * elements < 29);
        
        
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
        
        int temp = compress(selector, index, dgaps);
        fappend(compressed, temp);
        compressedwords++;
    }
    
    //compress(3, 35, dgaps);
    flexarray_print(compressed);
    printf("number of integers compressed: %d\n", arraysize);
    printf("compressed into %d words\n", compressedwords);
    
    printf("first compressed word in binary: \n");
    print_bigendian(compressed->items[0]);
    
    return 0;
}
