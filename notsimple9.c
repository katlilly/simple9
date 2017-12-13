/**********************************************************/
/*  notsimple9.c                                          */
/*  ------------                                          */
/*  A simple9-like compression scheme, using permutations */
/*  of the most common exceptions as well as uniform      */
/*  bitwidth selectors                                    */
/**********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "permutations.h"



int main(void)
{
    int numperms = 0;
    int mode = 1;
    int spread = 1;
    int exceptionfreq = 1;

        
    make_selector_table(mode, spread, exceptionfreq);
    
    printf("number of permutations: %d\n", numperms);
    


    return 0;
}
