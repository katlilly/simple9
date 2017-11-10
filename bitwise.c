#include<stdlib.h>
#include<stdio.h>
#include<limits.h>
#include<math.h>
#include<strings.h>

int compress(int selector, int index, int *dgaps) {
    selector = 3;
    index = index - (28/selector);
    int code, result = 0;
    int numcodes = 0;
    //  do {
    //   result += selector;
    //    printf("%b\n", result);
    return 1;
        
}

int main(void) {
    uint64_t h, selector, code, shiftedcode, result;
    int i;

    h = 3;
    //    printf("print 3 little endian:\n");
    //for(i = 0; i < 32; i++) {
    //    printf("%d", h%2);
    //    h = h >> 1;
    //}
    //printf("\n\n");

    h = 0x00ff0000;
    h = 3;
    printf("print 3 big endian:\n");
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


    printf("selector is 4\n");
    printf("first int is 7\n");
    selector = 4;
    code = 7;
    shiftedcode = code << 4;
    result = selector | shiftedcode;
    printf("selector: 0x%16llX\n code:       0x%16llX\n shifted code: 0x%16llX\n result:    0x%16llX\n", selector, code, shiftedcode, result);
    code = 5;
    shiftedcode = code << 8;
    result = result | shiftedcode;
    printf("selector: 0x%16llX\n code:        0x%16llX\n shifted code: 0x%16llX\n result:   0x%16llX\n", selector, code, shiftedcode, result);
    
  
    return 0;
}
