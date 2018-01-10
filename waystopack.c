#include <stdio.h>
#include <stdint.h>
#include <strings.h>


long long choose(n, k)
{
    if (k == 0) return 1;
    return (n * choose(n-1, k-1)) / k;
}
    



int main(void)
{
    int i;
    long long ways; /* every permutation */
    int payloadbits = 32;
    
    for (i = 1; i <= payloadbits; i++) {
        ways = choose(payloadbits - 1, i - 1);
        printf("ways to pack %d ints in %d bits: ", i, payloadbits);
        printf("%lld \t(%d bits)\n", ways, fls(ways));
    }
    
    return 0;
}
