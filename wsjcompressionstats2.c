#include<stdio.h>
#include<stdlib.h>
#include<strings.h>
#include<stdint.h>
#include "flexarray.h"
#include "fls.h"

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

    // convert list to dgaps
    prev = 0;
    //printf("converted to dgaps: \n");
    for (i = 0; i < length; i++) {
      dgaps[i] = postings_list[i] - prev;
      prev = postings_list[i];
      printf("%u, ", dgaps[i]);
    }
    printf("\n");


    // do the compression on one dgap list
    if (length == 1) {
      printf("not compressing single element lists\n");
    } else {
      while (index < arraysize) {
	elements = 0;
	maxbits = 0;
	index = numcompressedints;
        
	bitsused = 0;
	printf("chosing selector...\n");
	// changed check condition from 29 to 32 bits temporarily
	while (bitsused < 32) {
	  current = dgaps[index++];
	  bits = fls(current);
	  elements++;
	  if (bits > maxbits) {
	    maxbits = bits;
	  }
	  bitsused = maxbits * elements;
	}
	elements--;

	//changed check condition from 28 to 32 bits temporarily
	if (maxbits > 32) {
	  printf("maxbits: %u\n", maxbits);
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

	printf("ready to compress one word...\n");
	uint32_t temp = compress(selector, numcompressedints, dgaps);
	fappend(compresseddgaps, temp);
	printf("current value of compressedwords: %u\n", compressedwords);
	compressedwords++;
      }
      
      printf("%u integers were compressed into %d words\n", length, compressedwords);
    
    }//end while loop for reading in 
 
 

   
  }  
    
    
  //flexarray f = flexarray_new();
  //
  //while (1 == scanf("%d", &item)) {
  //    fappend(f, item);
  //}
  //arraysize = getsize(f);
  //flexarray_sort(f);
  //flexarray_print(f);
    
  docnums = malloc(arraysize * sizeof docnums[0]);
  for (i = 0; i < arraysize; i++) {
    docnums[i] = rand() % (arraysize * 100);
  }
    
  qsort(docnums, arraysize, sizeof docnums[0], compare_ints);
  
  
  
    
 
  //printf("first compressed word: \n");
  //print_binary(compresseddgaps->items[0]);
  //printf("0x%16llX\n", compresseddgaps->items[0]);
    
  // array to store decompressed numbers
  flexarray decompressed = flexarray_new();
  selectorfreqs = malloc(16 * sizeof selectorfreqs[0]);
  for (i = 0; i < 16; i++) {
    selectorfreqs[i] = 0;
  }
    
  // do the decompression
  numints = 0;
  for (i = 0; i < compressedwords; i++) {
    decompress(compresseddgaps->items[i], decompressed, numints);
  }
    
  // array to hold bit length frequencies
  for (i = 0; i < arraysize; i++) {
    gap = dgaps[i];
    if (gap > maxgap) {
      maxgap = gap;
    }
  }
  printf("maxgap: %d\n", maxgap);
  freqarraysize = fls(maxgap) + 1;
  int *bitlengthfreqs = malloc(freqarraysize * sizeof bitlengthfreqs[0]);
  for (i = 0; i < freqarraysize; i++) {
    bitlengthfreqs[i] = 0;
  }

  for (i = 0; i < freqarraysize; i++) {
    bitlengthfreqs[i] = 0;
  }
  for (i = 0; i < arraysize; i++) {
    bits = fls(dgaps[i]);
    if (bits < 2) { //won't be zeros in real data, ones should be run length encoded
      bits = 1;
      bitlengthfreqs[bits]++;
    } else if (bits < 17) {
      bitlengthfreqs[bits]++;
    } else if (bits < 32) {
      bits = 16;
      bitlengthfreqs[bits]++;
    } else if (bits == 32) {
      bitlengthfreqs[bits]++;
    } else {
      return(EXIT_FAILURE);
    }
  }
    
  printf("unrestricted bit lengths:\n");
  for (i = 0; i < freqarraysize; i++) {
    printf("number of dgaps requiring %u bits: %d\n", i, bitlengthfreqs[i]);
  }
    
  // print selector use stats
  for (i = 0; i < 16; i++) {
    printf("selector %u used %d times\n", i, selectorfreqs[i]);
  }
  uint32_t *wastedbits = malloc(compressedwords);
  for (i = 0; i < compressedwords; i++) {
    wastedbits[i] = countwastedbits(compresseddgaps->items[i]);
    //printf("%u bits wasted in %uth word\n", wastedbits[i], i);
  }
  int sumwasted = 0;
  for (i = 0; i < compressedwords; i++) {
    sumwasted += wastedbits[i];
  }
  int wastedperword = sumwasted / compressedwords;
  printf("average internally wasted bits per word: %d\n", wastedperword);
  double ratio = (double) compressedwords / (double) arraysize;
  printf("compression ratio: %.2f\n", ratio);
    
  //    for (i = 0; i < arraysize; i++) {
  //        printf("original: %u ", dgaps[i]);
  //        printf("decompressed: %u ", decompressed->items[i]);
  //        if (dgaps[i] != decompressed->items[i]) {
  //            printf("wrong");
  //        };
  //        printf("\n");
  //    }

  //have forgotten to free items in flexarrays... needs fixing
  flexarray_free(compresseddgaps);
  free(dgaps);
  free(docnums);
    
  return 0;
}
