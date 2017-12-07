#include <stdio.h>
#include <stdlib.h>
#include <string.h>



void output_perms(int *array, int length)
{
    int i;
    for (i = 0; i < length; i++) {
        printf("%d%c", array[i], i == length -1 ? '\n' : ' ');
    }
}
 

/* next lexicographical permutation
   taken from rosettacode */
int next_lex_perm(int *a, int n) {
#	define swap(i, j) {t = a[i]; a[i] = a[j]; a[j] = t;}
    int k, l, t;
 
    /* 1. Find the largest index k such that a[k] < a[k + 1]. If no such
       index exists, the permutation is the last permutation. */
    for (k = n - 1; k && a[k - 1] >= a[k]; k--);
    if (!k--) return 0;
 
    /* 2. Find the largest index l such that a[k] < a[l]. Since k + 1 is
       such an index, l is well defined */
    for (l = n - 1; a[l] <= a[k]; l--);
 
    /* 3. Swap a[k] with a[l] */
    swap(k, l);
 
    /* 4. Reverse the sequence from a[k + 1] to the end */
    for (k++, l = n - 1; l > k; l--, k++)
        swap(k, l);
    return 1;
#	undef swap
}
 
/* generates permutations in correct order and outputs unique ones
   taken from rosettacode */
void generate_perms(int *x, int n, int callback(int *, int))
{
    do {
        if (callback) callback(x, n);
    } while (next_lex_perm(x, n));
}
 


/* this removes some duplicate permutations but not all
 checking if each pair is not the same before swapping is not
 adequate because of the way this algorithm traverses the changes
 also this way does not output permutations in the order i want */ 
void generate_permutations_iterative_noduplicates(int n, int *array)
{
#define swap(i, j) {temp = array[i]; array[i] = array[j]; array[j] = temp;}
    int temp, numswaps = 0;
    int *c = malloc(n * sizeof *c);
    memset(c, 0, n * sizeof *c);

    output_perms(array, n);

    int i = 0;
    while (i < n) {
        if (c[i] < i) {
            if (i % 2 == 0) {
                if (array[0] != array[i]) {
                    swap(0, i);
                    output_perms(array, n);
                    numswaps++;
                }
            } else {
                if (array[c[i]] != array[i]) {
                    swap(c[i], i);
                    output_perms(array, n);
                    numswaps++;
                }
            }
            c[i] += 1;
            i = 0;
        } else {
            c[i] = 0;
            i += 1;
        }
    }
    printf("number of swaps performed: %d\n", numswaps);
#undef swap
}





int main(void)
{
    int i;
    int length = 5;
    
    int *array = malloc(length * sizeof *array);
    /* for (i = 0; i < length; i++) { */
    /*     array[i] = 3; */
    /* } */
    array[0] = 3;
    array[1] = 5;
    array[2] = 5;
    array[3] = 7;
    array[4] = 7;
    //    array[4] = 7;
    //printarray(length, array);
    

    //generate_permutations_iterative_noduplicates(length, array);

    //printf("rosetta method:\n");
    generate_perms(array, length, output_perms);
        
    return 0;
}
