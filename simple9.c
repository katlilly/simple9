#include<stdio.h>
#include<stdlib.h>
//#include<strings.h>
#include<stdint.h>
#include "mylib.h"
#include "flexarray.h"
#include "fls.h"

#define NUMBER_OF_DOCS (1024 * 1024 * 128)

static uint32_t *postings_list;
uint32_t *dgaps, *compressed, *decompressed;


typedef struct
{
  uint32_t bits;
  int numbers;
} selector;


selector table[] =
  {
    {1, 28},
    {2, 14},
    {3, 9},
    {4, 7},
    {5, 5},
    {7, 4},
    {9, 3},
    {14, 2},
    {28, 1}
  };


int number_of_selectors = sizeof(table) / sizeof(*table);


uint32_t min(uint32_t a, uint32_t b)
{
  return a <= b ? a : b;
}


uint32_t encode(uint32_t *destination, uint32_t *raw, uint32_t integers_to_compress)
{
  uint32_t which;                             // which line in selector table
  int current; 
  uint32_t *integer = raw;                    // the data to compress
  uint32_t *end = raw + integers_to_compress; // the end of the input array
  for (which = 0; which < number_of_selectors; which++)
  { // start by assuming we can fit 28 ints in
    end = raw + min(integers_to_compress, table[which].numbers);
    // drop out of inner loop either by reaching end of input array
    // or by having enough numbers to fill one word
    for (; integer < end; integer++)
    {
      if (fls(*integer) > table[which].bits)
      {
	which++; //drop to next line of selector table when needed
	// this should be a while loop? only drops down one if int doesn't fit
	break; // and return to outer loop
      }
    }
    if (fls(*integer) <= table[which].bits) // if number fits in bitwidth of selector
    {
      break; // return to outer loop without changing selector
    }
    
  }
  printf("chose selector: %d\n", table[which].bits);
  printf("number of ints for that selector: %d\n", table[which].numbers);
  *destination = 0; // initialize word to zero before packing ints and selector into it
  // need to set current to something other than zero when compressing more than one word
  for (current = 0; current < table[which].numbers; current++)
  {
    uint32_t value = current > integers_to_compress ? 0 : raw[current];
    *destination = *destination << table[which].bits | value; // pack ints into compressed word
  }
  *destination = *destination << 4 | which; // put the selector into compressed word
  return end - raw;    // return number of dgaps compressed so far.
}



void print_binary(uint32_t num) {
  int i;
  for (i = 31; i >= 0; i--) {
    if (num & (1<<i)) {
      printf("1");
    } else {
      printf("0");
    }
  }
  printf("\n");
}


// uses global variable <numints> to keep track of filling decompressed array
void decompress(uint32_t word, uint32_t *decompressed, int numints) {
  int i;
  uint32_t selector, mask, payload, temp;
  selector = word & 0xf;
  //selectorfreqs[selector]++;
  mask = (1 << (selector)) - 1;
  payload = word >> 4;
  for (i = 0; i < (28/selector) ; i++) {
    temp = payload & mask;
    decompressed[numints++] = temp;
    //fappend(decompressed, temp);
    payload = payload >> selector;
  }
}

int compare_ints(const void *a, const void *b) {
  const int *ia = (const int *) a;
  const int *ib = (const int *) b;
  return *ia < *ib ? -1 : *ia == *ib ? 0 : 1;
}

// this function repeats most of what the decompression function does
int countwastedbits(uint32_t word) {
  int i;
  uint32_t selector, mask, payload, temp, leadingzeros = 0;
  selector = word & 0xf;
  //selectorfreqs[selector]++;
  mask = (1 << (selector)) - 1;
  payload = word >> 4;
  for (i = 0; i < (28/selector) ; i++) {
    temp = payload & mask;
    payload = payload >> selector;
    leadingzeros += selector - fls(temp);
  }
  return leadingzeros;
}

int main(int argc, char *argv[]) {
  uint32_t i, prev = 0;
    
  //const char *filename;
  //    if (argc == 2) {
  //        filename = argv[1];
  //    } else {
  //        exit(printf("Usage::%s <binfile>\n", argv[0]));
  //    }
    
  postings_list = malloc(NUMBER_OF_DOCS * sizeof postings_list[0]);
  dgaps = malloc(NUMBER_OF_DOCS * sizeof dgaps[0]);
  compressed = malloc(NUMBER_OF_DOCS * sizeof compressed[0]);
  decompressed = malloc(NUMBER_OF_DOCS * sizeof decompressed[0]);
    
  //printf("Using: %s\n", filename);
  //    reading in of real data commented out until i get compress/decompress working again
  //    FILE *fp;
  //    if ((fp = fopen(filename, "rb")) == NULL) {
  //        exit(printf("Cannot open %s\n", filename));
  //    }
  //
  //    uint32_t length;
  //    while (fread(&length, sizeof(length), 1, fp)  == 1) {
  //        /*
  //         Read one postings list (and make sure we did so successfully)
  //         */
  //        // write one postings list into the postings_list array
  //        if (fread(postings_list, sizeof(*postings_list), length, fp) != length) {
  //            exit(printf("i/o error\n"));
  //        }
  //
  // print current postings list
  //        printf("%u: ", (unsigned)length);
  //        for (uint32_t *where = postings_list; where < postings_list + length; where++) {
  //            printf("%u ", (unsigned)*where);
  //        }
  //        printf("\n");
  //
  //        //convert postings list to dgaps list
  //        prev = 0;
  //        for (i = 0; i < length; i++) {
  //            dgaps[i] = postings_list[i] - prev;
  //            prev = postings_list[i];
  //        }

  // make some fake data for testing
  int length = 100;
  for (i = 0; i < length; i++)
  {
    postings_list[i] = rand() % 739;
  }
  qsort(postings_list, length, sizeof postings_list[0], compare_ints);
  prev = 0;
  for (i = 0; i < length; i++) {
    dgaps[i] = postings_list[i] - prev;
    prev = postings_list[i];
    if (dgaps[i] == 0) {
      dgaps[i] = 1;
    }
  }
  for (i = 0; i < length; i++) {
    printf("%d, ", dgaps[i]);
  }
  printf("\n");
    
  //  if (length < 2) {
  //  printf("not compressing lists of length 1\n");
  //} else {
    uint32_t encoded = 0;
    for (uint32_t *pos = dgaps; pos < dgaps + length; pos += encoded) {
      encoded += encode(compressed, pos, length); // returns number of ints compressed
      printf("number of ints encoded: %d\n", encoded);
      compressed++; //increment pointer into compressed array after each word encoded
    }
            
    //        compress(dgaps, length, compressed);
    //printf("compressed size: q%d\n", compressedsize);
        
    //}
  printf("first compressed word: \n");
  print_binary(compressed[0]);
  printf("0x%8X\n", compressed[0]);
      for (i = 0; i < length; i++) {
          printf("dgaps:  compressed:  decompressed\n");
          printf(" %d      %0x       %d\n", dgaps[i], compressed[i], decompressed[i]);
          printf("\n");
      }
    
  free(postings_list);
  free(dgaps);
  //free(compressed);
  free(decompressed);
    
  return 0;
}
