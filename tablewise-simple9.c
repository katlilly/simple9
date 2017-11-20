#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "fls.h"
//#include <vector>
//#include "maths.h"
//#include "asserts.h"
//#include "compress_integer_simple_9.h"


/*
   BITS_TO_USE
   -----------
   the number of bits that will be used to store an integer given the
   bit width of the integer. Index is bitwidth, value is bits/code.
*/
const size_t bits_to_use[] =
    {
        1,  1,  2,  3,  4,  5,  7,  7, 
        9,  9, 14, 14, 14, 14, 14, 28, 
        28, 28, 28, 28, 28, 28, 28, 28, 
        28, 28, 28, 28, 28, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64
    };

/*
  TABLE_ROW
  ---------
  The row of the table to use given the number of
  integers we can pack into the word. Index is number of ints can be
  packed, value at index is row of bits_to_use table
*/
const size_t table_row[] =
    {
        0, 1, 2, 3, 4, 4, 5, 5, 
        6, 6, 6, 6, 6, 7, 7, 7, 
        7, 7, 7, 7, 7, 7, 7, 7, 
        7, 7, 7, 8, 8
    };

/*
  COMPRESS_INTEGER_SIMPLE_9::SIMPLE9_SHIFT_TABLE
  ----------------------------------------------
  Number of bits to shift across when packing - is sum of prior packed ints
*/
const size_t simple9_shift_table[] =
    {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27,
        0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
        0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
        0, 4, 8, 12, 16, 20, 24, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
        0, 5, 10, 15, 20, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
        0, 7, 14, 21, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
        0, 9, 18, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
        0, 14, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
        0, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28
    };

/*
  COMPRESS_INTEGER_SIMPLE_9::INTS_PACKED_TABLE
  --------------------------------------------
  Number of integers packed into a 32-bit word, given its mask type
*/
const size_t ints_packed_table[] =
    {
        28, 14, 9, 7, 5, 4, 3, 2, 1
    };

/*
  COMPRESS_INTEGER_SIMPLE_9::CAN_PACK_TABLE
  -----------------------------------------
  Bitmask map for valid masks at an offset (column) for some num_bits_needed (row).
*/
const size_t can_pack_table[] =
    {
        0x01ff, 0x00ff, 0x007f, 0x003f, 0x001f, 0x000f, 0x000f, 0x0007, 0x0007, 0x0003, 0x0003, 0x0003, 0x0003, 0x0003, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001,
        0x01fe, 0x00fe, 0x007e, 0x003e, 0x001e, 0x000e, 0x000e, 0x0006, 0x0006, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x01fc, 0x00fc, 0x007c, 0x003c, 0x001c, 0x000c, 0x000c, 0x0004, 0x0004, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x01f8, 0x00f8, 0x0078, 0x0038, 0x0018, 0x0008, 0x0008, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x01f0, 0x00f0, 0x0070, 0x0030, 0x0010, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x01e0, 0x00e0, 0x0060, 0x0020, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x01c0, 0x00c0, 0x0040, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0180, 0x0080, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0100, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
    };

/*
  COMPRESS_INTEGER_SIMPLE_9::INVALID_MASKS_FOR_OFFSET
  ---------------------------------------------------
  We AND out masks for offsets where we don't know if we can fully pack for that offset
*/
const size_t invalid_masks_for_offset[] =
    {
        0x0000, 0x0100, 0x0180, 0x01c0, 0x01e0, 0x01f0, 0x01f0, 0x01f8, 0x01f8, 0x01fc, 0x01fc, 0x01fc, 0x01fc, 0x01fc, 0x01fe, 0x01fe, 0x01fe, 0x01fe, 0x01fe, 0x01fe, 0x01fe, 0x01fe, 0x01fe, 0x01fe, 0x01fe, 0x01fe, 0x01fe, 0x01fe, 0x01ff
    };

/*
  COMPRESS_INTEGER_SIMPLE_9::ROW_FOR_BITS_NEEDED
  ----------------------------------------------
  Translates the 'bits_needed' to the appropriate 'row' offset for use with can_pack table.
*/
const size_t row_for_bits_needed[] =
    {
        0, 0, 28, 56, 84, 112, 140, 140, 168, 168, 196, 196, 196, 196, 196, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224,			// Valid
        252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252		// Overflow
    };


/*
 MATHS_CEILING_LOG2_ANSWER[]
 ---------------------------
 */
/*!
 @brief Lookup table to compute ceil(log2(x))
 */
//static constexpr uint8_t maths_ceiling_log2_answer[0x100] =
static const uint8_t maths_ceiling_log2[0x100] =
{
    0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
};


int compare_ints(const void *a, const void *b) {
    const int *ia = (const int *) a;
    const int *ib = (const int *) b;
    return *ia < *ib ? -1 : *ia == *ib ? 0 : 1;
}



size_t encode(uint32_t *dest, size_t length, uint32_t *source, size_t source_integers)
{
    size_t words_in_compressed_string;
    uint32_t *into = dest;
    uint32_t *end = (uint32_t*)((uint8_t*)dest + length);
    size_t pos = 0;
    for (words_in_compressed_string = 0; pos < source_integers; words_in_compressed_string++)
        {
            /*
              Check for overflow (before we overflow)
            */
            if (into + 1 > end)
                return 0;

            size_t remaining = (pos + 28 < source_integers) ? 28 : source_integers - pos;
            size_t last_bitmask = 0x0000;
            size_t bitmask = 0xFFFF;

            /*
              Constrain last_bitmask to contain only bits for masks we can pack with
            */
            for (size_t offset = 0; offset < remaining && bitmask; offset++)
                {
                    bitmask &= can_pack_table[row_for_bits_needed[maths_ceiling_log2(source[pos + offset])] + offset];
                    //bitmask &= can_pack_table[row_for_bits_needed[maths::ceiling_log2(source[pos + offset])] + offset];
                    last_bitmask |= (bitmask & invalid_masks_for_offset[offset + 1]);
                }

            /*
              Ensure valid input (this is triggered when and integer greater than 2^28 is in the unput stream
            */
            if (last_bitmask == 0)
                return 0;

            /*
              Get position of lowest set bit
            */
            uint32_t mask_type = fls((uint32_t)last_bitmask);
            //uint32_t mask_type = maths::find_first_set_bit((uint32_t)last_bitmask);
            size_t num_to_pack = ints_packed_table[mask_type];

            /*
              Pack the word
            */
            *into = 0;
            size_t mask_type_offset = 28 * mask_type;
            for (size_t offset = 0; offset < num_to_pack; offset++)
                *into |= ((source[pos + offset]) << simple9_shift_table[mask_type_offset + offset]);
            *into = (*into << 4) | mask_type;
            pos += num_to_pack;
            into++;
        }

    return words_in_compressed_string * sizeof(*into);
}


int main(void) {

    uint32_t *source, *compressed, *decompressed;
    size_t length = 100, index = 0;
    int i, temp, prev;

    compressed = malloc(length * sizeof compressed[0]);
    decompressed = malloc(length * sizeof decompressed[0]);


    // start make fake data for testing
    source = malloc(length * sizeof source[0]);
    for (i = 0; i < length; i++) {
        source[i] = rand() % 972;
    }
    
    qsort(source, length, sizeof source[0], compare_ints);

    prev = 0;
    for (i = 0; i < length; i++) {
        temp = source[i] - prev;
        prev = source[i];
        source[i] = temp;
    }
    // end make fake data
    
    
    
    printf("%zu\n", bits_to_use[20]);

    return 0;
}
