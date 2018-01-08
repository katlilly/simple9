#include <stdio.h>
#include <stdlib.h>

int main(void) {

    int i;
    
    struct point {
        int x;
        int y;
        int array[10];
    } point1, point2;

    struct point point3 = {1, 2};

    printf("point 3 x coord: %d, y: %d\n", point3.x, point3.y);
    point1.y = 7;
    printf("%d\n", point1.y);

    struct point morepoints[5];
    for (i = 0; i < 5; i++) {
        morepoints[i].x = 8;
        morepoints[i].y = i;
    }
    printf("%d\n", morepoints[3].x);

    return 0;
}
