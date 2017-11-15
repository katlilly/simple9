#include<x86intrin.h>
#include<stdint.h>
#include<stdio.h>


void print128_int(__m128i var) {
    uint32_t *val = (uint32_t*) &var;
    //printf("%i %i %i %i\n", val[0], val[1], val[2], val[3]);
    printf("%0x %0x %0x %0x\n", val[0], val[1], val[2], val[3]);
}

void print128_long(__m128i var) {
    uint64_t *val = (uint64_t*) &var;
    printf("%llu %llu\n", val[0], val[1]);
    printf("%0llx %0llx\n", val[0], val[1]);
}


int main(void) {

    __m128i vector1 = _mm_set_epi32(1, 2, 3, 4);
    __m128i vector2 = _mm_set_epi32(5, 6, 7, 8);

    __m128i result = _mm_add_epi32(vector1, vector2);
    print128_int(result);

    print128_int(vector1);
    print128_int(vector2);
    vector1 = _mm_shuffle_epi32(vector1, _MM_SHUFFLE(2, 3, 0, 1));
    print128_int(vector1);


    //can use this function for integers?
    //move the upper 2 elements of b to lower 2 of dest, and
    //move upper 2 elements of a to upper 2 of dest
    __m128i vector4 = _mm_movehl_ps (vector1, vector2);
    print128_int(vector4);
    print128_long(vector4);
    
    //__m128i vector3 = _mm_set1_ps(4, 3, 2, 7);
    //vector3 = _mm_shuf_ps(vector3, vector3, _MM_SHUFFLE(0, 1, 2, 3));
    //print128_int(vector3);

    //__m64 a = _mm_set1_epi64(a);
    
    
    //uint64_t b = 12;
    //__m128i vector5 = _mm_setr_epi64(42, 43);
    //print128_long(vector5);

    return 0;
}
