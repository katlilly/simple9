#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <strings.h>

#define NUMBER_OF_DOCS (1024 * 1024 * 128)

static uint32_t *postings_list;

int numperms = 0;

typedef struct {
    int listNumber;
    int listLength;
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


/* return value is the number of possible permutations of the combination produced */
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
    //printf("total number of ints to pack: %d\n", numInts);
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
    //printf("\n");
    //printf("bits used: %d\n", checksum);

    
    /* sort the combination */
    //qsort(combination, numInts, sizeof(*combination), compare_ints);

    /* count the permutations and return that result */

    numperms = 0;
    generate_perms(combination, numInts, output_perms);
    //printf("number of permutations of this combination: %d\n", numperms);
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
   
    //printf("numMode: %d\n", numMode);
    //printf("mode: %d\n", mode);

    /* may want to add something here to check for wasted bits and
       use them by promoting a low to a mode or a mode to a high */
    /********** to do ************/

    
    /* fill an array with the combination */
    int numInts = inumHigh + numMode;
    //printf("numHigh: %d, numMode: %d, numInts: %d\n", inumHigh, numMode, numInts);
    //printf("total number of ints to pack: %d\n", numInts);
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
    //printf("\n");
    //printf("bits used: %d\n", checksum);

    
    /* sort the combination */
    //qsort(combination, numInts, sizeof(*combination), compare_ints);

    /* count the permutations and return that result */

    numperms = 0;
    generate_perms(combination, numInts, output_perms);
    //printf("number of permutations of this combination: %d\n", numperms);
    return numperms;
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

    int sum = 0;
    double fraction = 0;
    int max = 0;
    int mode;
    int lowexception;
    int nintyfifth;
    int set95th = 0;
    int highoutliers = 0, lowoutliers = 0;

    /* find mode and 95th percentile */
    //printf("bitwidth: \tnum ints: \tcumulative fraction:\n");
    for (i = 0; i < 10; i++) {
        sum += bitwidths[i];
        fraction = (double) sum / length;
        //printf("%d \t\t%d \t\t%.2f\n", i, bitwidths[i], fraction);
        if (bitwidths[i] > max) {
            max = bitwidths[i];
            mode = i;
        }
        if (set95th == 0 && fraction >= 0.95) {
            set95th = 1;
            nintyfifth = i;
        }
    }

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
        if (bitwidths[i] > max) {
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
        //if (length > 100 && listnumber < 100) {
        if (length > 10000) {
            //if (listnumber == 445139) {
        //if (listnumber == 96) {
        printf("list number: %d, length: %d\n", listnumber, (unsigned)length);
        //printf("getting list statistics\n");
            
        listStats stats = getStats(listnumber, length);
        //printf("mode: %d, lowexception: %d\n", stats.mode, stats.lowexception);

        if (stats.mode == 1 || stats.mode == stats.lowexception || stats.lowexception == 0) {
            printf("no low exception\n");
            /* make combinations without a low exception */
            perms = make_combs_withoutlow(stats.mode, stats.modalFraction, stats.highexception, stats.highFraction);
            printf("mode: %d, mode fraction: %.2f, high exception: %d, \npermutations: %d\n", stats.mode, stats.modalFraction, stats.highexception, perms);
        } else {
            //printf("using both low and high exceptions\n");
            perms = make_combs_withlow(stats.mode, stats.modalFraction, stats.lowexception, stats.lowFraction, stats.highexception, stats.highFraction);
            printf("mode: %d, mode fraction: %.2f, high exception: %d, \npermutations: %d\n", stats.mode, stats.modalFraction, stats.highexception, perms);
            /* make three bitwidth combinations */
        }

                    
        }/* end single list stats stuff */

       
        
      
    }/* end read-in of a single list*/
    
    return 0;
}
