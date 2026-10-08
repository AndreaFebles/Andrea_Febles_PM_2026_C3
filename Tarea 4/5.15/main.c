#include <stdio.h>
#include <stdlib.h>

void main(void)
{
    int X = 5, Y = 8, V[5] = {1, 3, 5, 7, 9};
    int *AY *AX;
    AY = &Y;
    X = *AY;
    *AY = V[3] + V[2];
    printf("\nX=%D")
}
