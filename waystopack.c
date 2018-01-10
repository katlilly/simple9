#include <stdio.h>
#include <stdint.h>
#include <strings.h>

/* function to calculate n choose k
   usage: n = payloadbits minus 1
          k = ints to pay minus 1 */
long long choose(n, k)
{
    if (k == 0) return 1;
    return (n * choose(n-1, k-1)) / k;
}
    

/* calculate number of possible ways to pack k ints into n bits */
int main(void)
{
    int i;
    long long ways; 
    int payloadbits = 32;
    
    for (i = 1; i <= payloadbits; i++) {
        ways = choose(payloadbits - 1, i - 1);
        printf("ways to pack %d ints in %d bits: ", i, payloadbits);
        printf("%lld \t(%d bits)\n", ways, fls(ways));
    }
    
    return 0;
}
