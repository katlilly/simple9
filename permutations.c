#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 this function will produce duplicate rows because of the repeated elements
 and won't output selector rows in the order i will need
 
 will probably be easiest to reorder write another function to tidy up
 the selector table outside of this function. ie sort it in the
 correct order and also remove duplicates
 */


void printarray(int length, int *array)
{
    for (int i = 0; i < length; i++) {
        printf("%d, ", array[i]);
    }
    printf("\n");
}
 

void print_permutations_recursive(int n, int *array)
{


}



void generate_permutations_iterative(int n, int *array)
{
    int temp, numswaps = 0;
    int *c = malloc(n * sizeof *c);
    memset(c, 0, n * sizeof *c);

    printarray(n, array);

    int i = 0;
    while (i < n) {
        if (c[i] < i) {
            if (i % 2 == 0) {
                temp = array[0];
                array[0] = array[i];
                array[i] = temp;
            } else {
                temp = array[c[i]];
                array[c[i]] = array[i];
                array[i] = temp;
            }
            printarray(n, array);
            numswaps++;
            c[i] += 1;
            i = 0;
        } else {
            c[i] = 0;
            i += 1;
        }
    }
    printf("number of swaps performed: %d\n", numswaps);
}


/* this removes some duplicate permutations but not all
 checking if each pair is not the same before swapping is not
 adequate because of the way this algorithm traverses the changes */ 
void generate_permutations_iterative_noduplicates(int n, int *array)
{
    int temp, numswaps = 0;
    int *c = malloc(n * sizeof *c);
    memset(c, 0, n * sizeof *c);

    printarray(n, array);

    int i = 0;
    while (i < n) {
        if (c[i] < i) {
            if (i % 2 == 0) {
                if (array[0] != array[i]) {
                    temp = array[0];
                    array[0] = array[i];
                    array[i] = temp;
                    printarray(n, array);
                    numswaps++;
                }
            } else {
                if (array[c[i]] != array[i]) {
                    temp = array[c[i]];
                    array[c[i]] = array[i];
                    array[i] = temp;
                    printarray(n, array);
                    numswaps++;
                }
            }
            //            printarray(n, array);
            //            numswaps++;
            c[i] += 1;
            i = 0;
        } else {
            c[i] = 0;
            i += 1;
        }
    }
    printf("number of swaps performed: %d\n", numswaps);
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
    //printarray(length, array);
    

    generate_permutations_iterative_noduplicates(length, array);
        
    return 0;
}
