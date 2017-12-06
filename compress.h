#ifndef COMPRESS_H_
#define COMPRESS_H_



/* data structure for each line in the selector table */
/* typedef struct selectorrec *selector; */


/* return minimum (or first if equal) of two input values */
extern uint32_t min(uint32_t a, uint32_t b);


/* simple 9 compression function */
extern uint32_t encode(uint32_t *destination, uint32_t *raw, uint32_t integers_to_compress);


/* returns number of ints decompressed */
extern uint32_t decompress(uint32_t *dest, uint32_t word, int offset);



/* this is adding wasted bits stats to a global array, need to change so it is
   passed a pointer instead */
extern uint32_t decompress_countwasted(uint32_t *dest, uint32_t word, int offset);



#endif

