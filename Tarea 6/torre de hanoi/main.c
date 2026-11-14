
#include <stdio.h>

void hanoi(int discos, char origen, char auxiliar, char destino) {

    if (discos == 1) {
        printf("Mover disco 1 de %c a %c\n", origen, destino);
        return;
    }

    hanoi(discos - 1, origen, destino, auxiliar);

    printf("Mover disco %d de %c a %c\n", discos, origen, destino);

    hanoi(discos - 1, auxiliar, origen, destino);
}

int main() {
    int discos;

    printf("TORRES DE HANOI\n");
    printf("Ingrese la cantidad de discos: ");
    scanf("%d", &discos);

    if (discos <= 0) {
        printf("Ingrese una cantidad valida de discos.\n");
    } else {
        printf("\nPasos para resolver las Torres de Hanoi:\n\n");

        hanoi(discos, 'A', 'B', 'C');
    }

    return 0;
}
