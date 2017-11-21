#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <strings.h>
//#include "fls.h"
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
const uint8_t maths_ceiling_log2[0x100] =
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


/*
 dest: compressed data array [out]
 length: size of output array [in] // what?
 source: the data to be compressed [in]
 source_integers: number of integers to be compressed [in]
 */
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
                    bitmask &= can_pack_table[row_for_bits_needed[fls(source[pos + offset])] + offset];
                    //bitmask &= can_pack_table[row_for_bits_needed[maths::ceiling_log2(source[pos + offset])] + offset];
                    last_bitmask |= (bitmask & invalid_masks_for_offset[offset + 1]);
                }

            /*
              Ensure valid input (this is triggered when and integer greater than 2^28 is in the input stream
            */
            if (last_bitmask == 0)
                return 0;

            /*
              Get position of lowest set bit (most significant bit?)
            */
            uint32_t mask_type = ffs((uint32_t)last_bitmask);
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

/*
 destination: the array to put decompressed data in [out]
 destination_integers: the number of integers to decode [in]
 source: the compressed data [in]
 source_length: size of compressed data (return value of encode) [in]
*/
 void decode(uint32_t *destination, size_t destination_integers, const uint32_t *source, size_t source_length)
{
    const uint32_t *compressed_sequence = (const uint32_t *) source;
    uint32_t *end = destination + destination_integers;
    
    while (destination < end)
    {
        uint32_t value = *compressed_sequence++;
        uint32_t mask_type = value & 0xF;
        value >>= 4;
        
        /*
         Unrolled loop to enable pipelining
         */
        switch (mask_type)
        {
            case 0x0:
                *destination++ = value & 0x1;
                *destination++ = (value >> 0x1) & 0x1;
                *destination++ = (value >> 0x2) & 0x1;
                *destination++ = (value >> 0x3) & 0x1;
                *destination++ = (value >> 0x4) & 0x1;
                *destination++ = (value >> 0x5) & 0x1;
                *destination++ = (value >> 0x6) & 0x1;
                *destination++ = (value >> 0x7) & 0x1;
                *destination++ = (value >> 0x8) & 0x1;
                *destination++ = (value >> 0x9) & 0x1;
                *destination++ = (value >> 0xA) & 0x1;
                *destination++ = (value >> 0xB) & 0x1;
                *destination++ = (value >> 0xC) & 0x1;
                *destination++ = (value >> 0xD) & 0x1;
                *destination++ = (value >> 0xE) & 0x1;
                *destination++ = (value >> 0xF) & 0x1;
                *destination++ = (value >> 0x10) & 0x1;
                *destination++ = (value >> 0x11) & 0x1;
                *destination++ = (value >> 0x12) & 0x1;
                *destination++ = (value >> 0x13) & 0x1;
                *destination++ = (value >> 0x14) & 0x1;
                *destination++ = (value >> 0x15) & 0x1;
                *destination++ = (value >> 0x16) & 0x1;
                *destination++ = (value >> 0x17) & 0x1;
                *destination++ = (value >> 0x18) & 0x1;
                *destination++ = (value >> 0x19) & 0x1;
                *destination++ = (value >> 0x1A) & 0x1;
                *destination++ = (value >> 0x1B) & 0x1;
                break;
            case 0x1:
                *destination++ = value & 0x3;
                *destination++ = (value >> 0x2) & 0x3;
                *destination++ = (value >> 0x4) & 0x3;
                *destination++ = (value >> 0x6) & 0x3;
                *destination++ = (value >> 0x8) & 0x3;
                *destination++ = (value >> 0xA) & 0x3;
                *destination++ = (value >> 0xC) & 0x3;
                *destination++ = (value >> 0xE) & 0x3;
                *destination++ = (value >> 0x10) & 0x3;
                *destination++ = (value >> 0x12) & 0x3;
                *destination++ = (value >> 0x14) & 0x3;
                *destination++ = (value >> 0x16) & 0x3;
                *destination++ = (value >> 0x18) & 0x3;
                *destination++ = (value >> 0x1A) & 0x3;
                break;
            case 0x2:
                *destination++ = value & 0x7;
                *destination++ = (value >> 0x3) & 0x7;
                *destination++ = (value >> 0x6) & 0x7;
                *destination++ = (value >> 0x9) & 0x7;
                *destination++ = (value >> 0xC) & 0x7;
                *destination++ = (value >> 0xF) & 0x7;
                *destination++ = (value >> 0x12) & 0x7;
                *destination++ = (value >> 0x15) & 0x7;
                *destination++ = (value >> 0x18) & 0x7;
                break;
            case 0x3:
                *destination++ = value & 0xF;
                *destination++ = (value >> 0x4) & 0xF;
                *destination++ = (value >> 0x8) & 0xF;
                *destination++ = (value >> 0xC) & 0xF;
                *destination++ = (value >> 0x10) & 0xF;
                *destination++ = (value >> 0x14) & 0xF;
                *destination++ = (value >> 0x18) & 0xF;
                break;
            case 0x4:
                *destination++ = value & 0x1F;
                *destination++ = (value >> 0x5) & 0x1F;
                *destination++ = (value >> 0xA) & 0x1F;
                *destination++ = (value >> 0xF) & 0x1F;
                *destination++ = (value >> 0x14) & 0x1F;
                break;
            case 0x5:
                *destination++ = value & 0x7F;
                *destination++ = (value >> 0x7) & 0x7F;
                *destination++ = (value >> 0xE) & 0x7F;
                *destination++ = (value >> 0x15) & 0x7F;
                break;
            case 0x6:
                *destination++ = value & 0x1FF;
                *destination++ = (value >> 0x9) & 0x1FF;
                *destination++ = (value >> 0x12) & 0x1FF;
                break;
            case 0x7:
                *destination++ = value & 0x3FFF;
                *destination++ = (value >> 0xE) & 0x3FFF;
                break;
            case 0x8:
                *destination++ = value & 0xFFFFFFF;
                break;
        }
    }
}

int main(void) {

    uint32_t *source, *compressed, *decompressed;
    size_t length = 20, index = 0;
    int i, temp, prev;

    compressed = malloc(length * sizeof compressed[0]);
    for (i = 0; i < length; i++) {
        compressed[i] = 0;
    }
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
       
    uint32_t *original = malloc(length * sizeof original[0]);
    for (i = 0; i < length; i++) {
        original[i] = source[i];
    }
    // end make fake data
    
    
    
    /*
     dest: compressed data array [out]
     length: size of output array [in] // what?
     source: the data to be compressed [in]
     source_integers: number of integers to be compressed [in]
     */
    
    size_t compressed_length = encode(compressed, length, source, length);
    //encode function is doing something to compressed array, but returning error code
    printf("return value of encode: %zu\n", compressed_length);
    printf("compressed data:\n");
    
    printf("decompressed data:\n");
    decode(decompressed, length, compressed, compressed_length);
    
    
    printf("original:    decompressed: \n");
    for (i = 0; i < length; i++) {
        printf("%3d          ", original[i]);
        printf("    %3d", decompressed[i]);
        if (original[i] != decompressed[i]) printf("  wrong");
        printf("\n");
    }
    
    
    
//    printf("%zu\n", bits_to_use[13]);

    return 0;
}
