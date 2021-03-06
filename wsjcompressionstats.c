#include<stdio.h>
#include<stdlib.h>
#include<strings.h>
#include<stdint.h>
#include "flexarray.h"
//#include "fls.h"

#define NUMBER_OF_DOCS (1024 * 1024 * 128)

static uint32_t *postings_list;

int compressedwords = 0;    //number of 32 bit compressed words
int numcompressedints = 0;  //keeps track of number of integers compressed
int numints = 0;            //keep track of number of integers while decompressing
int *selectorfreqs;         //values are set in decompression function, used for testing of              selector selection algorithm


struct flexarrayrec {
  uint32_t capacity;
  uint32_t itemcount;
  uint32_t *items;
};

flexarray flexarray_new() {
  flexarray result = malloc(sizeof *result);
  result->capacity = 2;
  result->itemcount = 0;
  result->items = malloc(result->capacity * sizeof result->items[0]);
  return result;
}

void flexarray_free(flexarray f) {
  free(f->items);
  free(f);
}

void fappend(flexarray f, uint64_t num) {
  if (f->itemcount == f->capacity) {
    f->capacity *= 2;
    f->items = realloc(f->items, f->capacity * sizeof f->items[0]);
  }
  f->items[f->itemcount++] = num;
}

void flexarray_print(flexarray f) {
  int i;
  for (i = 0; i < f->itemcount; i++) {
    printf("%u\n", f->items[i]);
  }
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

uint32_t compress(uint32_t selector, int thisindex, uint32_t *dgaps) {
  uint32_t code, shiftedcode, result = 0;
  int numcodes = 0;
  do {
    result = result | selector;
    code = dgaps[numcompressedints];
    //printf("0x%16llX\n", code);
    //printf("shifted code: \n");
    shiftedcode = code << (4 + (numcodes * selector));
    //printf("0x%16llX\n", shiftedcode);
    result = result | shiftedcode;
    //printf("current state of compressed word: \n");
    //printf("0x%16llX\n", result);
    numcompressedints++;
    numcodes++;
  } while (((numcodes + 1) * selector) < 29);
  return result;
}

// uses global variable <numints> to keep track of filling decompressed array
void decompress(uint32_t word, flexarray decompressed, int numints) {
  int i;
  uint32_t selector, mask, payload, temp;
  selector = word & 0xf;
  selectorfreqs[selector]++;
  mask = (1 << (selector)) - 1;
  payload = word >> 4;
  for (i = 0; i < (28/selector) ; i++) {
    temp = payload & mask;
    fappend(decompressed, temp);
    payload = payload >> selector;
  }
}

int compare_ints(const void *a, const void *b) {
  const int *ia = (const int *) a;
  const int *ib = (const int *) b;
  //return (*ia > *ib) - (*ia < *ib);
  return *ia < *ib ? -1 : *ia == *ib ? 0 : 1;
}

// this function repeats most of what the decompression function does
int countwastedbits(uint32_t word) {
  int i;
  uint32_t selector, mask, payload, temp, leadingzeros = 0;
  selector = word & 0xf;
  selectorfreqs[selector]++;
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
  uint32_t selector;
  uint32_t arraysize = 5000, i, prev = 0;
  uint32_t *docnums, *dgaps;
  uint32_t bits, maxbits, current, elements;
  uint32_t index = 0;
  int freqarraysize, bitsused;
  int gap = 0, maxgap = 0;
    
  flexarray compresseddgaps = flexarray_new();

  const char *filename;

  if (argc == 2) {
    filename = argv[1];
  } else {
    exit(printf("Usage::%s <binfile>\n", argv[0]));
  }

  postings_list = malloc(NUMBER_OF_DOCS * sizeof postings_list[0]);
  dgaps = malloc(NUMBER_OF_DOCS * sizeof dgaps[0]);
  
  //printf("Using: %s\n", filename);
  FILE *fp;
  if ((fp = fopen(filename, "rb")) == NULL) {
    exit(printf("Cannot open %s\n", filename));
  }

  uint32_t length;
  while (fread(&length, sizeof(length), 1, fp)  == 1) {
    /*
      Read one postings list (and make sure we did so successfully)
    */
    if (fread(postings_list, sizeof(*postings_list), length, fp) != length) {
      exit(printf("i/o error\n"));
    }
    printf("%u: ", (unsigned)length);

  
    // write docnums of current postings list into the array
    for (uint32_t *where = postings_list; where < postings_list + length; where++) {
      printf("%u ", (unsigned)*where);
    }
    printf("\n");

    //convert docnums to dgaps
    prev = 0;
    for (i = 0; i < length; i++) {
      dgaps[i] = postings_list[i] - prev;
      prev = postings_list[i];
    }


    
    compressedwords = 0;  
    while (index < length) {
        elements = 0;
        maxbits = 0;
        index = numcompressedints;
        
        bitsused = 0;
        while (bitsused < 29) {
            current = dgaps[index++];
            bits = fls(current);
            elements++;
            if (bits > maxbits) {
                maxbits = bits;
            }
            bitsused = maxbits * elements;
        }
        elements--;
        
        if (maxbits > 28) {
            return(EXIT_FAILURE);
        } else if (maxbits > 14) {
            selector = 28;
        } else if (maxbits > 9) {
            selector = 14;
        } else if (maxbits > 7) {
            selector = 9;
        } else if (maxbits > 5) {
            selector = 7;
        } else {
            selector = maxbits;
        }
       
        uint64_t temp = compress(selector, numcompressedints, dgaps);
        fappend(compresseddgaps, temp);
        compressedwords++;
    }

    
    printf("%u integers were compressed into %d words\n", length, compressedwords);
    
    
  }//end while


  return 0;
}
