#ifndef PERMUTATIONS_H_
#define PERMUTATIONS_H_

#include <stddef.h>


extern int compare_ints(const void *a, const void *b);

extern void output_perms(int *array, int length);
 
extern int next_lex_perm(int *a, int n);

extern void generate_perms(int *x, int n, void callback(int *, int));
 
extern int * make_combination(int mode, int spread, int excpfreq);

extern void make_selector_table(int mode, int spread, int exceptionfreq);


#endif
