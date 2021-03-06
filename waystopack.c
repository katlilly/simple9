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

/* print to screen each permutation */
void output_perms(int *array, int length)
{
    int i;
    for (i = 0; i < length; i++) {
        //    printf("%d%c", array[i], i == length - 1 ? '\n' : ' ');
    }
}
 

/* get next lexicographical permutation. taken from rosettacode */
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
   taken directly from rosettacode */
void generate_perms(int *x, int n, void callback(int *, int))
{
    do {
        if (callback) callback(x, n);
        numperms++;
    } while (next_lex_perm(x, n));
}



/* return 1 if a bigger, -1 if b bigger, 0 if equal */
int compare_ints(const void *a, const void *b) {
    const int *ia = (const int *) a;
    const int *ib = (const int *) b;
    return *ia < *ib ? -1 : *ia == *ib ? 0 : 1;
}



/* designed to output a combination array, but for now just returns
   the number of possible permutations of the combination produced */
int make_combs_withlow(int mode, double modFrac, int low, double lowFrac, int high, double highFrac)
{
    /* decide how many low and high exceptions to include */
    int approxNumInts = 32 / mode;
    double dnumLow = lowFrac * approxNumInts;
    int inumLow = dnumLow + 0.5; 
    double dnumHigh = highFrac * approxNumInts;
    int inumHigh = dnumHigh + 0.5;

    /* decide number of modal bitwidths */
    int sumExcp = inumHigh * high + inumLow * low;
    int numMode = 0, sum = sumExcp;
    while (sum < 32) {
        sum += mode;
        numMode++;
    }
    if (sum > 32) numMode--;
    
    
    /* may want to add something here to check for wasted bits and
       use them by promoting a low to a mode or a mode to a high */
    /********** to do ************/

    
    /* fill an array with the combination */
    int numInts = inumLow + inumHigh + numMode;
    printf("total number of ints to pack: %d\n", numInts);
    int * combination = malloc(numInts * sizeof(*combination));

    int i;
    for (i = 0; i < inumLow; i++) {
        combination[i] = low;
    }
    for (i = inumLow; i < inumLow + numMode; i++) {
        combination[i] = mode;
    }
    for (i = inumLow + numMode; i < inumLow + numMode + inumHigh; i++) {
        combination[i] = high;
    }
    int checksum = 0;
    for (i = 0; i < numInts; i++) {
        printf("%d, ", combination[i]);
        checksum += combination[i];
    }
    printf("\n");
    printf("bits used: %d\n", checksum);

    
    /* sort the combination */
    //qsort(combination, numInts, sizeof(*combination), compare_ints);

    /* count the permutations and return that result */

    numperms = 0;
    generate_perms(combination, numInts, output_perms);
    printf("number of permutations of this combination: %d\n", numperms);
    return numperms;
}


int make_combs_withoutlow(int mode, double modFrac, int high, double highFrac)
{
    /* decide how many low and high exceptions to include */
    int approxNumInts = 32 / mode;
    //printf("approx ints to pack: %d\n", approxNumInts);
    double dnumHigh = highFrac * approxNumInts;
    int inumHigh = dnumHigh + 0.5;
    //printf("high ints to pack: %d\n", inumHigh);

    /* decide number of modal bitwidths */
    int sumExcp = inumHigh * high;
    int numMode = 0, sum = sumExcp;
    while (sum <= 32) {
        sum += mode;
        numMode++;
    }
    if (sum > 32) numMode--;
   
    printf("numMode: %d\n", numMode);
    printf("mode: %d\n", mode);

    /* may want to add something here to check for wasted bits and
       use them by promoting a low to a mode or a mode to a high */
    /********** to do ************/

    
    /* fill an array with the combination */
    int numInts = inumHigh + numMode;
    printf("total number of ints to pack: %d\n", numInts);
    int * combination = malloc(numInts * sizeof(*combination));

    int i;
    for (i = 0; i < numMode; i++) {
        combination[i] = mode;
    }
    for (i = numMode; i < numMode + inumHigh; i++) {
        combination[i] = high;
    }
    int checksum = 0;
    for (i = 0; i < numInts; i++) {
        printf("%d, ", combination[i]);
        checksum += combination[i];
    }
    printf("\n");
    printf("bits used: %d\n", checksum);

    
    /* sort the combination */
    //qsort(combination, numInts, sizeof(*combination), compare_ints);

    /* count the permutations and return that result */

    numperms = 0;
    generate_perms(combination, numInts, output_perms);
    printf("number of permutations of this combination: %d\n", numperms);
    return numperms;
}



/* old version, starting from scratch with new statistical inputs. */
void make_restricted_combination(int mode, int stdev, int excepfreq)
{
    int i, j, k, l, sum, numints, high, low;

    high = mode + stdev;
    low = mode - stdev;

    /* remove low exception when it doesn't make sense */
    if (mode - stdev <= 0) {
        low = mode;
    }

    for (i = 1; i <= excepfreq; i++) {
        for (j = 1; j < 32; j++) {
            for (k = 1; k <= excepfreq; k++) {
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
                        //      printf("%d ", comb[l]);
                    }
                    //printf("\n");
                    
                    /* send this combination to generate perms function */
                    /* may want to silence the printing in the output_perms function */
                    generate_perms(comb, numints, output_perms);
                }
            }
        }
    }

}



int main(void)
{
    int mode, stdev, excepfreq;
    long long ways; 
    int payloadbits = 32;

    int perms = make_combs_withlow(5, 0.5, 4, 0.3, 7, 0.2);
    printf("%d\n", perms);

    perms = make_combs_withlow(6, 0.5, 5, 0.3, 7, 0.2);
    printf("%d\n", perms);

    
    perms = make_combs_withoutlow(2, 0.8, 3, 0.2);
    printf("%d\n", perms);
    
    /* printf("ways to pack with mode of 3 and stdev of 2\n"); */
    /* printf("1 bit ints, 3 bit ints, 5 bit ints, ints packed\n"); */
    /* make_restricted_combination(3, 2); */

    /* this puts no limitations on exception frequency, so way too many
       selectors when modal bitwidth is small */
    /* for (mode = 1; mode < 10; mode++) { */
    /*     for (stdev = 1; stdev < 4; stdev++) { */
    /*         for (excepfreq = 1; excepfreq < 5; excepfreq++) { */
    /*             numperms = 0; */
    /*             make_restricted_combination(mode, stdev, excepfreq);  */
    /*             printf("number of permutations with mode = %d, stdev = %d, excepfreq = %d: %d\n", mode, stdev, excepfreq, numperms); */
    /*         } */
    /*     } */
    /* } */

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
