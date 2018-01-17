#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <strings.h>
#include <math.h>

#define NUMBER_OF_DOCS (1024 * 1024 * 128)

static uint32_t *postings_list;

int numperms = 0;

typedef struct {
    int listNumber;
    int listLength;
    double mean;
    double stdev;
    int mode;
    int lowexception;
    int highexception;
    double modalFraction;
    double lowFraction;
    double highFraction;
    int numPerms;
} listStats;

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


int * make_combs(int mode, double modeFrac, int low, double lowFrac, int high, double highFrac)
{
    int bitsused, numInts, i;
    int *combination;

    int payload = 32;
    int numLow = 1, numMode = 1, numHigh = 1;
    double lowusedfrac, modeusedfrac, highusedfrac;

    /* deal with very short lists manually */
    if (mode > 16) {
        int comb[1] = {32};
        return combination;
    }
    if (mode > 10 || low + high + mode > 32) {
        int comb[2] = {16, 16};
        return combination;
    }
    /* if ints to pack is 3, and high <= 10, don't use output combination,
       instead use 10, 10, 12 or 10, 11, 11 */
    

    /* count correct number of each bitwidth to use */
    numInts = numLow + numMode + numHigh;
    bitsused = low + mode + high;
    lowusedfrac = (double) numLow / numInts; 
    modeusedfrac = (double) numMode / numInts;
    highusedfrac = (double) numHigh / numInts;
    int tries = 0;
    while (bitsused < 32) {
        tries++;
        if (highusedfrac < highFrac && bitsused + high <= 32) {
            numHigh++;
            bitsused += high;
            numInts++;
            highusedfrac = (double) numHigh / numInts;
        }
        if (modeusedfrac < modeFrac && bitsused + mode <= 32) {
            numMode++;
            bitsused += mode;
            numInts++;
            modeusedfrac = (double) numMode / numInts;
        }
        if (lowusedfrac < lowFrac && bitsused + low <= 32) {
            numLow++;
            bitsused += low;
            numInts++;
            lowusedfrac = (double) numLow / numInts;
        }
        /* ideal way to break out of loop */
        if (bitsused + low > 32) {
            break;
        }
        /* add extra ints to fill space regardless of modfrac, catches a few
           cases that can't break out because of exception frequencies */
        if (tries > 16) {
            if (bitsused + mode <= 32) {
                numMode++;
                bitsused += mode;
            } else if (bitsused + low <= 32) {
                numLow++;
                bitsused += low;
            } else {
                break;
            }
        }
     }

    /* check for errors */
    int external_wasted_bits = 32 - numLow * low - numMode * mode - numHigh * high;
    if (external_wasted_bits > low) {
        printf("%d wasted bits  ", external_wasted_bits);
    }
    if (bitsused > 32) {
        printf("overflow  ");
    }
    
    /* fill combination array */
    numInts = numLow + numHigh + numMode;
    combination = malloc(numInts * sizeof(*combination));
    for (i = 0; i < numLow; i++) {
        combination[i] = low;
    }
    for (i = numLow; i < numLow + numMode; i++) {
        combination[i] = mode;
    }
    for (i = numLow + numMode; i < numInts; i++) {
        combination[i] = high;
    }
    //printf("bitsused: %d, ", bitsused);
    for (i = 0; i < numInts; i++) {
        //printf("%d%c", combination[i], i == numInts - 1 ? '\n' : ',');
    }

    return combination;
}


/* this function takes values from stats struct and correctly decides whether both high
   and low exceptions are needed, but doesn't deal with fractions correctly, goes by fraction of bits
   used instead of fraction of ints packed */
int * make_combs_3widths(int mode, double modFrac, int low, double lowFrac, int high, double highFrac)
{
    int bitsused, numInts, i;
    double usedFrac;
    int *combination;

    /* set size of payload */
    int payload = 32;    

    /* deal with very short lists manually */
    if (mode > 16) {
        printf("32\n");
        return combination;
    }

    if (mode > 10) {
        printf("16, 16\n");
        return combination;
    }

    
    
    /* start with one of each bitwidth included */
    int numHigh = 1, numMode = 1, numLow = 0;
    if (lowFrac > 0) {
        numLow = 1;
    }

    /* count high exceptions needed */
    bitsused = high;
    usedFrac = (double) bitsused / payload;
    while (usedFrac < highFrac) {
        numHigh++;
        bitsused += high;
        usedFrac = (double) bitsused / payload;
    }

    /* count low exceptions needed */
    bitsused += low;
    usedFrac = (double) bitsused / payload;
    while (usedFrac < highFrac + lowFrac) {
        numLow++;
        bitsused += low;
        usedFrac = (double) bitsused / payload;
    }

    /* count modes needed to fill up payload */
    bitsused += mode;
    while (usedFrac < 1) {
        numMode++;
        bitsused += mode;
        usedFrac = (double) bitsused / payload;
    }

    
    /* remove one or more mode if we overshot */
    while (usedFrac > 1) {
        if (numMode > 0) {
            numMode--;
            bitsused -= mode;
            usedFrac = (double) bitsused / payload;
        } else {
            printf("crapped out on list with mode %d\n", mode);
            break;
        }

    }

    numInts = numMode + numLow + numHigh;

    combination = malloc(numInts * sizeof(*combination));
    
    for (i = 0; i < numLow; i++) {
        combination[i] = low;
    }
    for (i = numLow; i < numLow + numMode; i++) {
        combination[i] = mode;
    }
    for (i = numLow + numMode; i < numInts; i++) {
        combination[i] = high;
    }

    for (i = 0; i < numInts; i++) {
        printf("%d%c ", combination[i], i == numInts - 1 ? '\n' : ',');
    }
    
    return combination;
    
}




/* getStats function calculate statistics of a list for use in selector generator
   returns a listStats structure, takes list number and length */
listStats getStats(int number, int length)
{
    listStats tempList;
    tempList.listNumber = number;
    tempList.listLength = length;

    int i;
    int *bitwidths = malloc(32 * sizeof(bitwidths[0]));
    for (i = 0; i < 32; i++) {
        bitwidths[i] = 0;
    }
            
    int *dgaps = malloc(length * sizeof(dgaps[0]));
    int prev = 0;

    for (i = 0; i < length; i++) {
        dgaps[i] = postings_list[i] - prev;
        prev = postings_list[i];
        bitwidths[fls(dgaps[i])]++;
    }
    //printf("\n");

    double sum = 0;
    double mean;
    double stdev = 0;

    for (i = 0; i < length; i++) {
        sum += fls(dgaps[i]);
    }
    mean = sum / length;

    for (i = 0; i < length; i++) {
        stdev += pow(fls(dgaps[i]) - mean, 2);
    }

    stdev = sqrt(stdev/length);
    //printf("mean: %.3f, stdev: %.3f\n", mean, stdev);
    tempList.mean = mean;
    tempList.stdev = stdev;
    
    sum = 0;
    double fraction = 0;
    int max = 0;
    int mode;
    int lowexception;
    int nintyfifth;
    int set95th = 0;
    int highoutliers = 0, lowoutliers = 0;

    /* find mode and 95th percentile */
    //printf("bitwidth: \tnum ints: \tcumulative fraction:\n");
    for (i = 0; i < 32; i++) {
        sum += bitwidths[i];
        fraction = (double) sum / length;
        //printf("%d \t\t%d \t\t%.2f\n", i, bitwidths[i], fraction);
        if (bitwidths[i] >= max) {
            max = bitwidths[i];
            mode = i;
        }
        if (set95th == 0 && fraction >= 0.95) {
            set95th = 1;
            nintyfifth = i;
        }
    }

    //printf("\n\n");
    
    /* find exception frequencies */            
    for (i = 0; i < mode; i++) {
        lowoutliers += bitwidths[i];
    }
    for (i = mode + 1; i < 32; i++) {
        highoutliers += bitwidths[i];
    }

    /* find next most frequent bitwidth smaller than the mode */
    max = 0;
    for (i = 0; i < mode; i++) {
        if (bitwidths[i] >= max) {
            max = bitwidths[i];
            lowexception = i;
        } 
    }

    tempList.mode = mode;
    tempList.lowexception = lowexception;
    tempList.highexception = nintyfifth;
    tempList.modalFraction = (double) bitwidths[mode] / length;
    tempList.lowFraction = (double) lowoutliers / length;
    tempList.highFraction = (double) highoutliers / length;
    
    return tempList;
}


int main(int argc, char *argv[])
{
    int listnumber = 0;
    int *combination;
    
    const char *filename;

    if (argc == 2) {
        filename = argv[1];
    } else {
        exit(printf("Usage::%s <binfile>\n", argv[0]));
    }

    postings_list = malloc(NUMBER_OF_DOCS * sizeof postings_list[0]);

    //printf("Using: %s\n", filename);
    FILE *fp;
    if ((fp = fopen(filename, "rb")) == NULL) {
	exit(printf("Cannot open %s\n", filename));
    }

    uint32_t length;
    while (fread(&length, sizeof(length), 1, fp)  == 1) {

        /* Read one postings list (and make sure we did so successfully) */
        if (fread(postings_list, sizeof(*postings_list), length, fp) != length) {
            exit(printf("i/o error\n"));
        }
        //printf("%u: ", (unsigned)length);
        listnumber++;

        int perms;
        //if (listnumber == 236029) {

            //for (int i = 0; i < length; i++) {
            //    printf("%d, ", postings_list[i]);
            //}
        //if (listnumber == 499520) {
            
        //if (length > 1000) {
            //if (listnumber == 445139) {
            //if (listnumber == 96) {
            //printf("list number: %d, length: %d\n", listnumber, (unsigned)length);
            //printf("getting list statistics\n");
            
        listStats stats = getStats(listnumber, length);

        
        //make_combs_3widths(3, 0.5, 2, 0.2, 6, 0.3);
        //("listnumber: %d, length: %d, combination: ", listnumber, length);
        combination = make_combs(stats.mode, stats.modalFraction, stats.lowexception, stats.lowFraction, stats.highexception, stats.highFraction);

        int topack = sizeof(combination) / sizeof(*combination);
        /* above line doesn't do what i want */
        printf("list number %d, ints to pack: %d\n", listnumber, topack);
        for (int i = 0; i < topack; i++) {
            //printf("%d,", combination[i]);
        }
            
            
            /* output statistics to a csv file to look at with matlab */
          
            
             /* statistics struct     */
            /*************************/
            /* int listNumber;       */
            /* int listLength;       */
            /* double mean;          */
            /* double stdev;         */
            /* int mode;             */
            /* int lowexception;     */
            /* int highexception;    */
            /* double modalFraction; */
            /* double lowFraction;   */
            /* double highFraction;  */
            /* int numPerms;         */
            /*************************/
            
            //printf("mode: %d, lowexception: %d\n", stats.mode, stats.lowexception);

            //if (stats.mode == 1 || stats.mode == stats.lowexception || stats.lowexception == 0) {
                //printf("no low exception\n");
                //stats.numPerms = make_combs_withoutlow(stats.mode, stats.modalFraction, stats.highexception, stats.highFraction);
                //printf("mode: %d, mode fraction: %.2f, high exception: %d, \npermutations: %d\n", stats.mode, stats.modalFraction, stats.highexception, perms);
            //} else {
            //stats.numPerms = make_combs_withlow(stats.mode, stats.modalFraction, stats.lowexception, stats.lowFraction, stats.highexception, stats.highFraction);
                //printf("mode: %d, mode fraction: %.2f, high exception: %d, \npermutations: %d\n", stats.mode, stats.modalFraction, stats.highexception, perms);

                //printf("%d, %4d,  %.3f, %.3f, %2d, %2d, %2d, %.5f, %.5f, %.5f, %d\n", listnumber, length, stats.mean, stats.stdev, stats.mode, stats.lowexception, stats.highexception, stats.modalFraction, stats.lowFraction, stats.highFraction, stats.numPerms);
            //}


            
                    
            //}/* end single list stats stuff */

       
        
      
    }/* end read-in of a single list*/
    
    return 0;
}
