#include <stdio.h>
#include <stdint.h>
#include <strings.h>
#include <stdlib.h>


int mode = 5, stdev = 2;
int numperms = 0;

/* function to calculate n choose k
   usage: n = payloadbits minus 1
          k = ints to pay minus 1 */
long long choose(n, k)
{
    if (k == 0) return 1;
    return (n * choose(n-1, k-1)) / k;
}

void output_perms(int *array, int length)
{
    int i;
    for (i = 0; i < length; i++) {
        //    printf("%d%c", array[i], i == length - 1 ? '\n' : ' ');
    }
}
 

/* next lexicographical permutation. taken from rosettacode */
int next_lex_perm(int *a, int n) {
#define swap(i, j) {t = a[i]; a[i] = a[j]; a[j] = t;}
    int k, l, t;
 
    /* 1. Find the largest index k such that a[k] < a[k + 1]. If no such
       index exists, the permutation is the last permutation. */
    for (k = n - 1; k && a[k - 1] >= a[k]; k--);

    if (!k--) return 0; 
    /* 2. Find the largest index l such that a[k] < a[l]. Since k + 1 is
       such an index, l is well defined */
    for (l = n - 1; a[l] <= a[k]; l--) {
        ;
    }
 
    /* 3. Swap a[k] with a[l] */
    swap(k, l);
 
    /* 4. Reverse the sequence from a[k + 1] to the end */
    for (k++, l = n - 1; l > k; l--, k++)
        swap(k, l);
    return 1;
#undef swap
}


/* generates permutations in correct order and outputs unique ones
   taken from rosettacode */
void generate_perms(int *x, int n, void callback(int *, int))
{
    do {
        if (callback) callback(x, n);
        numperms++;
    } while (next_lex_perm(x, n));
}





void make_restricted_combination(int mode, int stdev)
{
    int i, j, k, l, sum, numints, high, low;

    high = mode + stdev;
    low = mode - stdev;
    
    if (mode - stdev <= 0) {
        printf("invalid bitwidths\n");
        return;
        
    }

    /* this calculates all the combinations, doesn't count the permutations */
    for (i = 1; i < 32; i++) {
        for (j = 1; j < 32; j++) {
            for (k = 1; k < 32; k++) {
                sum = i * low + j * mode + k * high;
                /* if sum = 32 we've found a valid combination */
                if (sum == 32) {
                    numints = i + j + k;
                    /* print the valid combination */
                    //printf("%2d %2d %2d\t%d\n", i, j, k, numints);
                    /* fill an array with the given combination*/
                    int *comb = malloc(sizeof(comb[0]) * numints);
                    for (l = 0; l < i; l++) {
                        comb[l] = low;
                    }
                    for (l = i; l < i + j; l++) {
                        comb[l] = mode;
                    }
                    for (l = i + j; l < i + j + k; l++) {
                        comb[l] = high;
                    }
                    for (l = 0; l < numints; l++) {
                        //  printf("%d ", comb[l]);
                    }
                    //printf("\n");
                    /* send this combination to generate perms function */
                    generate_perms(comb, numints, output_perms);
                }
            }
        }
    }

}



int main(void)
{
    int i;
    long long ways; 
    int payloadbits = 6;

    
    /* printf("ways to pack with mode of 3 and stdev of 2\n"); */
    /* printf("1 bit ints, 3 bit ints, 5 bit ints, ints packed\n"); */
    /* make_restricted_combination(3, 2); */

    for (i = 1; i < 10; i++) {
        numperms = 0;
        make_restricted_combination(i, 1);
        printf("number of permutations with mode = %d: %d\n", i, numperms);
    }

    /* make_restricted_combination(5, 1); */
    /* printf("%d ways to pack with mode = 5, stdev = 1\n", numperms); */
    /* printf("\n"); */

    /* make_restricted_combination(5, 2); */
    /* printf("%d ways to pack with mode = 5, stdev = 2\n", numperms); */
    
    /* count all the ways to pack n ints (checked no duplications)
       with no restrictions on bitwidths, this gives way too many options */
    /* for (i = 1; i <= payloadbits; i++) { */
    /*     ways = choose(payloadbits - 1, i - 1); */
    /*     printf("ways to pack %d ints in %d bits: ", i, payloadbits); */
    /*     printf("%lld \t(%d bits)\n", ways, fls(ways)); */
    /* } */
    
    return 0;
}
