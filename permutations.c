#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int numperms;

int compare_ints(const void *a, const void *b) {
    const int *ia = (const int *) a;
    const int *ib = (const int *) b;
    return *ia < *ib ? -1 : *ia == *ib ? 0 : 1;
}


void output_perms(int *array, int length)
{
    int i;
    for (i = 0; i < length; i++) {
        printf("%d%c", array[i], i == length - 1 ? '\n' : ' ');
    }
}
 

/* next lexicographical permutation
   taken from rosettacode */
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
 


/* this removes some duplicate permutations but not all
 checking if each pair is not the same before swapping is not
 adequate because of the way this algorithm traverses the changes
 also this way does not output permutations in the order i want */ 
void generate_permutations_iterative_noduplicates(int n, int *array)
{
#define swap(i, j) {temp = array[i]; array[i] = array[j]; array[j] = temp;}
    int i, temp, numswaps = 0;
    int *c = malloc(n * sizeof *c);
    memset(c, 0, n * sizeof *c);

    output_perms(array, n);

    i = 0;
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



/* exception frequencies: 1 = low, 2 = medium, 3 = all exceptions */

int * make_combination(int mode, int spread, int excpfreq)
{
    int i, sum, numints, highexcp, lowexcp;
    /* always use 8 bits for selectors?
       start by assuming 8 bits for selectors? */
    /* int **table = malloc(256 * sizeof **table); */
    int *result = malloc(24 * sizeof *result);
    memset(result, 0, 24 * sizeof *result); 

        
    highexcp = mode + spread;
    lowexcp = mode - spread;
    
    /* single exception case */
    /* ********************* */
    if (excpfreq == 1) {
        /* output low exceptions */
        if (lowexcp > 0) {
            result[0] = lowexcp;
            sum = lowexcp;
            for (i = 1; sum <= 24; i++) {
                if (sum + mode <=24) {
                    result[i] = mode;
                }
                sum += mode;
                numints = i;
            }
            /* is necessary to sort before generating perms or won't get them all */
            qsort(result, numints, sizeof *result, compare_ints);
            generate_perms(result, numints, output_perms);
        }
        
        /* output high exceptions */
        result[0] = highexcp;
        sum = highexcp;
        for (i = 1; sum <= 24; i++) {
            if (sum + mode <=24) {
                result[i] = mode;
            }
            sum += mode;
            numints = i;
        }
        /* is necessary to sort before generating perms or won't get them all */
        qsort(result, numints, sizeof *result, compare_ints);
        generate_perms(result, numints, output_perms);
    } /* end single exception case */

    
    /* frequent exceptions case */
    /* ************************ */
    if (excpfreq == 3) {
        sum = 0;
        if (lowexcp > 0) {
            printf("low exception is non-zero\n");
            /* low exception valid, add each of three repeatedly */
            result[0] = highexcp;
            sum = highexcp;
            for (i = 1; sum <= 24; i += 3) {
                if (sum + lowexcp <= 24) {
                    result[i] = lowexcp;
                    sum += lowexcp;
                    numints = i+1;
                } else break;
                if (sum + mode <= 24) {
                    result[i+1] = mode;
                    sum += mode;
                    numints = i+2;
                } else break;
                if (sum + highexcp <= 24) {
                    result[i+2] = highexcp;
                    sum += highexcp;
                    numints = i+3;
                } else if (sum + mode <= 24) {
                    result[i+2] = mode;
                    sum += mode;
                    numints = i+3;
                } else if (sum + lowexcp <= 24) {
                    result [i+2] = lowexcp;
                    sum += lowexcp;
                    numints = i+3;
                }
            } /* end three bitwidths, high frequency exception case */

        } else {
            /* low exception invalid, add each of two repeatedly */
            result[0] = highexcp;
            sum = highexcp;
            for (i = 1; i <= 24; i += 2) {
                if (sum + mode <= 24) {
                    result[i] = mode;
                    sum += mode;
                    numints = i + 1;
                } else break;
                if (sum + highexcp <= 24) {
                    result[i+1] = highexcp;
                    sum += highexcp;
                    numints = i + 2;
                } else if (sum + mode <= 24) {
                    result[i+1] = mode;
                    sum += mode;
                    numints = i + 2;
                }
            } /* end two bitwidths, high frequency exception case */ 
        }
        for (i = 0; i < numints; i++) {
            printf("%d%c", result[i], i == numints - 1 ? '\n' : ' ');
        }
        qsort(result, numints, sizeof *result, compare_ints);
        generate_perms(result, numints, output_perms);
    } /* end high frequency exception case */ 
    
    return result;
}

void make_selector_table(int mode, int spread, int exceptionfreq)
{
    int i, j;
    int *combination;
        for (i = 1; i <= mode; i++) {
            for (j = 1; j*i <= 24; j++) {
                printf("%d ", i);
            }
            printf("\n");
        }
    
        combination = make_combination(mode, spread, exceptionfreq);

        /* these below lines can be done better, probably should specify each line
28           manually when i > 4 */
        for (i = mode + 1; i < 24; i++) {
        for (j = 1; j*i <= 24; j++) {
            printf("%d ", i);
        }
        printf("\n");
    }
}
