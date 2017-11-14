#include<x86intrin.h>
#include<stdint.h>
#include<stdio.h>

void print128_num(__m128i var) {
  uint32_t *val = (uint32_t*) &var;
  printf("%i %i %i %i\n", val[0], val[1], val[2], val[3]);
}

int main(void) {

__m128i vector1 = _mm_set_epi32(1, 2, 3, 4);
__m128i vector2 = _mm_set_epi32(0xffffffff, 8, 9, 0);

__m128i result = _mm_add_epi32(vector1, vector2);
print128_num(result);

print128_num(vector1);
print128_num(vector2);
vector1 = _mm_shuffle_epi32(vector1, 2);
print128_num(vector1);

//vector2 = _mm_shuf_ps(vector2, vector2, _MM_SHUFFLE(0, 1, 2, 3));

//print128_num(vector2);
return 0;
}
