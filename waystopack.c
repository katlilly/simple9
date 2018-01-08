#include <stdio.h>
#include <stdint.h>

struct twoints {
    int a;
    int b;
};

struct twoints twointways[20];

int payloadbits = 32;

int getcombinations(int intstopack) {
    int numcombinations = 0;

    if (intstopack < 1 || intstopack > 32) {
        printf("invalid number of ints to pack\n");
        return EXIT_FAILURE;
    }
    if (intstopack == 1) {
        return 
    }

    
    // this logic only works for packing two ints
    for (int i = 1; i < payloadbits / intstopack; i++) {
        printf("bits for int a: %d, bits for int b: %d\n", i, 32-i);
        numcombinations++;
    }
    printf("number of ways to pack %d ints: %d\n", intstopack, numcombinations);
    return numcombinations;
}

int main(void)
    {

    int i, combinations, permutations;

    for (i = 1; i <= payloadbits; i++) {
        printf("number of ways to pack %d ints:\n", i);
        combinations = getcombinations(i);
        
        
    }

    for (i = 0; i < 20; i++) {
        twointways[i].a = i+1;
        twointways[i].b = payloadbits - twointways[i].a;
        combinations++;
    }
    
    printf("number of combinations: %d\n", combinations);

    

    return 0;
}
