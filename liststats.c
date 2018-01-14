#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <strings.h>

#define NUMBER_OF_DOCS (1024 * 1024 * 128)

static uint32_t *postings_list;

typedef struct {
    int listNumber;
    int listLength;
    int mode;
    int lowexception;
    int highexception;
    double modalFraction;
    double lowFraction;
    double highFraction;
} listStats;


/* calculate statistics of a list for use in selector generator
   returns a listStats structure */
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
    printf("\n");

    int sum = 0;
    double fraction = 0;
    int max = 0;
    int mode;
    int nintyfifth;
    int set95th = 0;
    int highoutliers = 0, lowoutliers = 0;

    /* find mode and 95th percentile */
    printf("bitwidth: \tnum ints: \tcumulative fraction:\n");
    for (i = 0; i < 10; i++) {
        sum += bitwidths[i];
        fraction = (double) sum / length;
        printf("%d \t\t%d \t\t%.2f\n", i, bitwidths[i], fraction);
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
    int lowexception;
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
    int i;
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

        
        if (listnumber == 96) {
            printf("list number: %d, length: %d\n", listnumber, (unsigned)length);
            printf("getting list statistics\n");
            
            listStats statistics = getStats(listnumber, length);

            
        }/* end single list stats stuff */
        
      
    }/* end read-in of a single list*/

    
    return 0;
}
