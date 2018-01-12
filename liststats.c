#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <strings.h>

#define NUMBER_OF_DOCS (1024 * 1024 * 128)

static uint32_t *postings_list;


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
                //printf("%d, ", dgaps[i]);
            }
            printf("\n");

            double sum = 0;
            double percent = 0;
            int max = 0;
            double mode;
            int fifth, nintyfifth;
            int set5th = 0, set95th = 0;
            //int tenth, nintieth;
            //int set10th = 0, set90th = 0;
            double highoutliers = 0, lowoutliers = 0;

            printf("bitwidth: \tfrequency: \tcumulative:\n");
            for (i = 0; i < 10; i++) {
                sum += bitwidths[i];
                percent = sum / length;
                printf("%d \t\t%d \t\t%f\n", i, bitwidths[i], percent);
                /* find mode */
                if (bitwidths[i] > max) {
                    max = bitwidths[i];
                    mode = i;
                }
                /* find 5th percentile */
                if (set5th == 0 && percent >= 0.05) {
                    set5th = 1;
                    fifth = i;
                }
                /* find 95th percentile */
                if (set95th == 0 && percent >= 0.95) {
                    set95th = 1;
                    nintyfifth = i;
                }
                /* /\* find 10th percentile *\/ */
                /* if (set10th == 0 && percent >= 0.1) { */
                /*     set10th = 1; */
                /*     tenth = i; */
                /* } */
                /* /\* find 90th percentile *\/ */
                /* if (set90th == 0 && percent >= 0.9) { */
                /*     set90th = 1; */
                /*     nintieth = i; */
                /* } */
            }

            printf("sum: %f\n", sum);

            
           
            printf("mode: %f\n", mode);
            printf("5th percentile: %d\n", fifth);
            printf("95th percentil: %d\n", nintyfifth);
            //printf("10th percentile: %d\n", tenth);
            //printf("90th percentil: %d\n", nintieth);

            
            for (i = 0; i < mode; i++) {
                lowoutliers += bitwidths[i];
            }
            for (i = mode + 1; i < 32; i++) {
                highoutliers += bitwidths[i];
            }


            /* 5th percentile is not the right number to chose for the low exception
               how about next-most-common lower than mode? */
            max = 0;
            int lowexception;
            for (i = 0; i < mode; i++) {
                if (bitwidths[i] > max) {
                    max = bitwidths[i];
                    lowexception = i;
                } 
            }

            double highoutlierfraction = highoutliers / length;
            double lowoutlierfraction = lowoutliers / length;
            double modalfraction = bitwidths[mode] / length;
            printf("%.0f of %d ints (%f) are larger than the mode\n", highoutliers, length, highoutlierfraction);
            printf("%.0f of %d ints (%f) are smaller than the mode\n", lowoutliers, length, lowoutlierfraction);

            printf("values chosen for to make selectors with:\n");
            printf("low exception: %d, mode: %f, high exception: %d\n", lowexception, mode, nintyfifth);

            printf("%f \n", modalfraction);
            
            //printf("sum of mode and outliers: %d\n", bitwidths[mode] + highoutliers + lowoutliers);
            
        }/* end single list stats stuff */
        
      
    }/* end read-in of a single list*/

    
    return 0;
}
