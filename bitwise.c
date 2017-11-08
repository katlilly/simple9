#include<stdlib.h>
#include<stdio.h>
#include<limits.h>
#include<math.h>
#include<strings.h>

int main(void) {
  unsigned int a, b, c, d, e, f, g, h;
  int i;

  a = 0xff0000ff;
  b = 0x0000ff00;
  c = a && b;
  d = 0xffffffff;
  e = 0b11111111;
  f = 0b1111111000000111;
  g = 1 << 8;
  //h = UINT_MAX;
  h = 0x00ff0000;
  //h = pow(2,16);
  //h = 0b110001111001;
  
  printf("a: %u, b: %u, c: %u, d: %u\n", a, b, c, d);

  printf("e: %u, f: %u, g: %u, h: %u\n\n", e, f, g, h);

  
  printf("print 0x00ff0000 little endian:\n");
  for(i = 0; i < 32; i++) {
    printf("%d", h%2);
    h = h >> 1;
  }
  printf("\n\n");

  h = 0x00ff0000;
  
  printf("print 0x00ff0000 big endian:\n");
  for (i = 31; i >= 0; i--) {
    if (h & (1<<i)) {
      printf("1");
    } else {
      printf("0");
    }
  }
  printf("\n");

  
  printf("fls of 8 returns: %d\n", fls(8));
  printf("fls of 7 returns: %d\n", fls(7));
  
  
  return 0;
}
