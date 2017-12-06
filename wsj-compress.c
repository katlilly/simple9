/****************************************************************
 *                                                              *
 * wsj-compress -- reads in all the postings lists from the wsj *
 * postings.bin file, compressesed them all, decompresses them, *
 * and checks for errors.                                       *
 *                                                              *
 * Usage:                                                       *
 *       make wsj-compress                                      *
 *       make do-wsj-compress | grep wrong                      *
 *       make do-wsj-compress > compressionratios.txt           *
 *                                                              *
 ****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <stdint.h>
#include "compress.h"

#define NUMBER_OF_DOCS (1024 * 1024 * 1)
#define MAX_BITWIDTH 32
#define MAX_LENGTH 157871
#define NUMBER_OF_LISTS 499693


int main(int argc, char *argv[])
{
    int listnumber = 0;
    int offset;
    double total_raw_length = 0;
    double total_compressed_length = 0;
    uint32_t i, prev, length;
    uint32_t numencoded;       /* return value of encode function, the number of ints compressed */
    uint32_t compressedwords;  /* offset for position in output array "compressed" */
    uint32_t compressedints;   /* offset for position in input array "dgaps" */

    const char *filename;
    FILE *fp;

    uint32_t *postings_list = malloc(NUMBER_OF_DOCS * sizeof *postings_list);
    uint32_t *dgaps = malloc(NUMBER_OF_DOCS * sizeof *dgaps);
    uint32_t *compressed = malloc(NUMBER_OF_DOCS * sizeof *compressed);
    uint32_t *decoded = malloc(NUMBER_OF_DOCS * sizeof *decoded);
        
    
    /* get filename from command line and open file for reading */
    if (argc == 2) {
        filename = argv[1];
    } else {
        exit(printf("Usage::%s <binfile>\n", argv[0]));
    }
    /* printf("Using: %s\n", filename); */
    if ((fp = fopen(filename, "rb")) == NULL) {
        exit(printf("Cannot open %s\n", filename));
    }

    
    /* read in postings list and compress/decompress each one */
    while (fread(&length, sizeof(length), 1, fp)  == 1) {
        
        /* Read one postings list (and make sure we did so successfully) */
        if (fread(postings_list, sizeof(*postings_list), length, fp) != length) {
            exit(printf("i/o error\n"));
        }
        listnumber++;

        total_raw_length += length;
        
        /* convert current postings list to a dgaps list */
        prev = 0;
        for (i = 0; i < length; i++) {
            dgaps[i] = postings_list[i] - prev;
            prev = postings_list[i];
        }
               

        /* compress this postings list */
        numencoded = 0;           /* return value of encode function, the number of ints compressed */
        compressedwords = 0;      /* offset for position in output array "compressed" */
        compressedints = 0;       /* offset for position in input array "dgaps" */
        for (compressedints = 0; compressedints < length; compressedints += numencoded) {
            numencoded = encode(compressed + compressedwords, dgaps + compressedints, length - compressedints);
            compressedwords++;
        }

        total_compressed_length += compressedwords;
       
        
        /* decompress the current postings list */
        /* ************************************ */
        offset = 0;  /* reset number of decompressed ints to zero for each word */
        for (i = 0; i < compressedwords; i++) {
            offset += decompress(decoded, compressed[i], offset);
        }

       
                
        /* find errors in compression or decompression 
         ******************************************* */
        /* printf("original: decompressed:\n"); */
        for (i = 0; i < length; i++) {
            /* printf("%6d        %6d", dgaps[i], decoded[i]); */
            if (dgaps[i] != decoded[i]) {
                printf(" wrong");
            }
            /* printf("\n"); */

            
        }
        /* printf("successfully compressed and decompressed list %d\n", listnumber); */

        /* print compression ratios */
        /* human-readable version */
        /* printf("raw length: %6.1f compressed length: %6.1f\n", total_raw_length, total_compressed_length); */

        /* print csv version of compression ratio stats */
        if (listnumber < 500) {
            printf("%d, %d, %d\n", listnumber, length, compressedwords);
        }

    }/* end read-in of postings lists */

    printf("compression ratio of entire set of lists: %.3f\n", total_compressed_length / total_raw_length);
  

    free(postings_list);
    free(dgaps);
    free(compressed);
    free(decoded);
     
    return 0;
}
