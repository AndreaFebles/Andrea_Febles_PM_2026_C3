#include <stdio.h>
#include <stdlib.h>

int productoria(int);

void main(void)
{
    int NUM;
    {
        printf("Ingresa el numero del cual quieres calcular la poductoria: ");
        scanf("%d", &NUM);
    }
    while (NUM >100 || NUM < 1);
    printf("\nLa prooductoria de %d", NUM, productoria(NUM));
}
int productoria(int N)
{
    int I, PRO = 1;
    for (I = 1; I <= N; I++)
        PRO *= I;
    return (PRO);
}
