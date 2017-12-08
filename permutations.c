#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int compare_ints(const void *a, const void *b) {
    const int *ia = (const int *) a;
    const int *ib = (const int *) b;
    return *ia < *ib ? -1 : *ia == *ib ? 0 : 1;
}


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



/* exception frequencies: 1 = low, 2 = medium, 3 = all exceptions */

int * make_combination(int mode, int spread, int excpfreq)
{
    /* always use 8 bits for selectors?
       start by assuming 8 bits for selectors? */
    /* int **table = malloc(256 * sizeof **table); */
    int *result = malloc(24 * sizeof *result);
    memset(result, 0, 24 * sizeof *result); 
    int i, sum, numints;
        
    int highexcp = mode + spread;
    int lowexcp = mode - spread;
    

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
        printf("entered frequent exception case\n");
        sum = 0;
        if (lowexcp > 0) {
            printf("using low exception\n");
            //add each of three repeatedly
            result[0] = mode;
            sum = mode;
            for (i = 1; sum <= 24; i += 3) {
                printf("i: %d\n", i);
                if (sum + lowexcp > 24) {
                    
                    break;
                } else {
                    printf("adding: %d, sum: %d\n", lowexcp, sum);
                    result[i] = lowexcp;
                    sum += lowexcp;
                }
                if (sum + mode > 24) {
                
                    break;
                } else {
                    printf("adding: %d, sum: %d\n", mode, sum);
                    result[i+1] = mode;
                    sum += mode;
                }
                if (sum + highexcp > 24) {
                
                    break;
                } else {
                    printf("adding: %d, sum: %d\n", highexcp, sum);
                    result[i+2] = highexcp;
                    sum += highexcp;
                }
            }

        } else {
            //add each of two repeatedly
            printf("not using low exception\n");
            result[0] = mode;
            sum = mode;
            for (i = 1; sum <= 24; i += 2) {
                if (sum + mode > 24) {
                    break;
                } else {
                    result[i] = mode;
                    sum += mode;
                }
                if (sum + highexcp > 24) {
                    break;
                } else {
                    result[i+1] = highexcp;
                    sum += highexcp;
                }
            }

        }
        qsort(result, numints, sizeof result[0], compare_ints);
        generate_perms(result, numints, output_perms);
    }
    
    
    
    return result;
}

void make_selector_table(int mode, int spread, int exceptionfreq)
{
        int i, j;
        for (i = 1; i < mode; i++) {
            for (j = 1; j*i <= 24; j++) {
                printf("%d ", i);
            }
            printf("\n");
        }
    
        int *combination = make_combination(mode, spread, exceptionfreq);

        /* these below lines can be done better, probably should specify each line
           manually when i > 4 */
        for (i = mode + 1; i < 24; i++) {
        for (j = 1; j*i <= 24; j++) {
            printf("%d ", i);
        }
        printf("\n");
    }
}

int main(void)
{
    int i, j;
    int length = 5;
    
    /* int *array = malloc(length * sizeof *array); */
    /* /\* for (i = 0; i < length; i++) { *\/ */
    /* /\*     array[i] = 3; *\/ */
    /* /\* } *\/ */
    /* array[0] = 3; */
    /* array[1] = 5; */
    /* array[2] = 5; */
    /* array[3] = 7; */
    /* array[4] = 7; */
    //    array[4] = 7;
    //printarray(length, array);
    

    //generate_permutations_iterative_noduplicates(length, array);

    //printf("rosetta method:\n");
    //generate_perms(array, length, output_perms);
    //int ** table = make_selector_table(4, 2, 3);

    //int *combination = make_selector_table(1, 1, 1);
    //make_selector_table(1, 1, 1);

    int mode = 4;
    int spread = 2;
    int exceptionfreq = 3;

    make_selector_table(mode, spread, exceptionfreq);
    
    
    


    return 0;
}
