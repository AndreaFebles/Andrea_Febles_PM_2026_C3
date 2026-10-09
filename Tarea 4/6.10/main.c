#include <stdio.h>
#include <stdlib.h>

const int MAX = 50;
void Cuadrado(int [][MAX], int);
void Imprime (int [][MAX], int);

void main (void)
{
    int CMA [MAX][MAX],TAM;
    do
    {
        printf("Ingrese el tamaño impar de la matriz: ");
        scanf("%d", &TAM);
    }
    while ((TAM > MAX || TAM < 1) && (TAM % 2));
    Cuadrado (CMA, TAM);
    Imprime(CMA, TAM);
}

void Cuadrado(int A )
