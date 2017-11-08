#include <stdio.h>
#include <stdlib.h>
#include "mylib.h"
#include "flexarray.h"

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
    
/* currently insertion sort, should be using improved merge sort */
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


