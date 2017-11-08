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

//just case c for now, three bit codes
int compress(int selector, int index, int *dgaps) {
    int code, result = 0;
    int numcodes = 0;
    while ((numcodes * selector) < 28) {
        result += selector; //put the selector code in
        code = dgaps[index];
        //code = code << (4 + numcodes * selector);
        result = result & (code << (4 + numcodes * selector));
        index++;
    }
    //printf("compressing with selector %c\n", selector);
    //compressedwords++;
    
    return result;
}


int main(void) {
    int selector;
    int item, arraysize, i, prev = 0;
    int *dgaps;
    int bits, maxbits, current, elements, index = 0;
    
    
    flexarray f = flexarray_new();
    
    while (1 == scanf("%d", &item)) {
        fappend(f, item);
    }
    
    arraysize = getsize(f);
    flexarray_sort(f);
    flexarray_print(f);
    
    dgaps = emalloc(arraysize * sizeof dgaps[0]);
    
    for (i = 0; i < arraysize; i++) {
        dgaps[i] = 0;
    }
    
    for (i = 0; i < arraysize; i++) {
        dgaps[i] = f->items[i] - prev;
        prev = f->items[i];
    }
    
    printf("d-gaps:\n");
    for (i = 0; i < arraysize; i++) {
        printf("%d\n", dgaps[i]);
    }
    
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
        
        compress(selector, index, dgaps);
    }
    
    printf("number of integers compressed: %d\n", arraysize);
    printf("compressed into %d words\n", compressedwords);
    
    return 0;
}
