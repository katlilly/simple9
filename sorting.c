#include<stdlib.h>
#include<stdio.h>

int compare_ints(const void *a, const void *b) {
  const int *ia = (const int *) a;
  const int *ib = (const int *) b;
  return (*ia > *ib) - (*ia < *ib);
}
  
  
int main(void) {
  int arraysize = 100;
  int *array;
  int i;
    
  array = malloc(arraysize * sizeof array[0]);

  for (i = 0; i < 100; i++) {
    array[i] = rand() % 200;
  }

  for (i = 0; i < 100; i++) {
    printf("%d, ", array[i]);
  }

  qsort(array, arraysize, sizeof array[0], compare_ints);
  
  for (i = 0; i < 100; i++) {
    printf("%d, ", array[i]);
  }
  printf("\n");
  
  return 0;
}
