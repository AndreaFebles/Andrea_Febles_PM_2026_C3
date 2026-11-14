
#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *archivo;
    char *texto;
    int capacidad = 10;
    int longitud = 0;
    int caracter;

    texto = (char *)malloc(capacidad * sizeof(char));

    if (texto == NULL) {
        printf("Error al asignar memoria.\n");
        return 1;
    }

    archivo = fopen("texto.txt", "r");

    if (archivo == NULL) {
        printf("No se pudo abrir el archivo.\n");
        free(texto);
        return 1;
    }

    while ((caracter = fgetc(archivo)) != EOF) {

        if (longitud >= capacidad - 1) {
            capacidad *= 2;

            char *temp = (char *)realloc(texto, capacidad * sizeof(char));

            if (temp == NULL) {
                printf("Error al ampliar la memoria.\n");
                free(texto);
                fclose(archivo);
                return 1;
            }

            texto = temp;
        }

        texto[longitud] = (char)caracter;
        longitud++;
    }

    texto[longitud] = '\0';

    printf("\nContenido del archivo:\n");
    printf("%s\n", texto);

    fclose(archivo);
    free(texto);

    return 0;
}

