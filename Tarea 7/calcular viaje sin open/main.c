
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define ARCHIVO "vehiculos.dat"

typedef struct {
    int id;
    char nombre[50];

    double consumoCiudad;
    double consumoCarretera;

    double seguroAnual;
    double neumaticosKm;
    double mantenimientoKm;

    double valorInicial;
    double valorFinal;
    double vidaUtil;

    double impuestosAnuales;
    double limpieza;

    double kmAnuales;
} Vehiculo;

Vehiculo *vehiculos = NULL;
int cantidad = 0;
int siguienteID = 1;


void leerTexto(char *destino, int tamano) {
    fgets(destino, tamano, stdin);
    destino[strcspn(destino, "\n")] = '\0';
}

double leerNumero(const char *mensaje) {
    char entrada[100];
    char *fin;
    double numero;

    while (1) {
        printf("%s", mensaje);
        leerTexto(entrada, sizeof(entrada));

        numero = strtod(entrada, &fin);

        while (*fin == ' ' || *fin == '\t')
            fin++;

        if (entrada[0] != '\0' && *fin == '\0'
            && isfinite(numero)) {
            return numero;
        }

        printf("Entrada no valida. Intente nuevamente.\n");
    }
}

int leerEntero(const char *mensaje) {
    double numero;

    while (1) {
        numero = leerNumero(mensaje);

        if (numero >= 0 && numero <= 2147483647
            && floor(numero) == numero) {
            return (int)numero;
        }

        printf("Debe introducir un numero entero positivo.\n");
    }
}

double leerPositivo(const char *mensaje) {
    double numero;

    do {
        numero = leerNumero(mensaje);

        if (numero < 0)
            printf("El valor no puede ser negativo.\n");

    } while (numero < 0);

    return numero;
}


void cargarVehiculos() {
    FILE *archivo = fopen(ARCHIVO, "rb");

    if (archivo == NULL)
        return;

    Vehiculo temporal;

    while (fread(&temporal, sizeof(Vehiculo), 1, archivo) == 1) {

        Vehiculo *nuevo = realloc(
            vehiculos,
            (cantidad + 1) * sizeof(Vehiculo)
        );

        if (nuevo == NULL) {
            printf("Error de memoria al cargar los datos.\n");
            free(vehiculos);
            vehiculos = NULL;
            cantidad = 0;
            break;
        }

        vehiculos = nuevo;
        vehiculos[cantidad] = temporal;
        cantidad++;

        if (temporal.id >= siguienteID)
            siguienteID = temporal.id + 1;
    }

    fclose(archivo);
}

void guardarVehiculos() {
    FILE *archivo = fopen(ARCHIVO, "wb");

    if (archivo == NULL) {
        printf("No se pudieron guardar los datos.\n");
        return;
    }

    if (cantidad > 0) {
        fwrite(vehiculos, sizeof(Vehiculo), cantidad, archivo);
    }

    fclose(archivo);
}


int buscarVehiculo(int id) {
    for (int i = 0; i < cantidad; i++) {
        if (vehiculos[i].id == id)
            return i;
    }

    return -1;
}


void registrarDatos(Vehiculo *v) {

    printf("\nNombre o modelo del vehiculo: ");
    leerTexto(v->nombre, sizeof(v->nombre));

    while (strlen(v->nombre) == 0) {
        printf("El nombre no puede estar vacio: ");
        leerTexto(v->nombre, sizeof(v->nombre));
    }

    printf("\n--- CONSUMO DE COMBUSTIBLE ---\n");

    v->consumoCiudad = leerPositivo(
        "Consumo en ciudad (L/100 km): "
    );

    v->consumoCarretera = leerPositivo(
        "Consumo en carretera (L/100 km): "
    );

    printf("\n--- COSTOS ANUALES ---\n");

    v->seguroAnual = leerPositivo(
        "Costo del seguro anual: "
    );

    v->impuestosAnuales = leerPositivo(
        "Costo de impuestos anuales: "
    );

    printf("\n--- COSTOS POR KILOMETRO ---\n");

    v->neumaticosKm = leerPositivo(
        "Costo de neumaticos por km: "
    );

    v->mantenimientoKm = leerPositivo(
        "Costo de mantenimiento por km: "
    );

    printf("\n--- DEPRECIACION ---\n");

    v->valorInicial = leerPositivo(
        "Valor inicial del vehiculo: "
    );

    do {
        v->valorFinal = leerPositivo(
            "Valor final estimado: "
        );

        if (v->valorFinal > v->valorInicial)
            printf("El valor final no puede superar el inicial.\n");

    } while (v->valorFinal > v->valorInicial);

    do {
        v->vidaUtil = leerPositivo(
            "Vida util total del vehiculo (anios): "
        );

        if (v->vidaUtil <= 0)
            printf("La vida util debe ser mayor que cero.\n");

    } while (v->vidaUtil <= 0);

    printf("\n--- OTROS COSTOS ---\n");

    v->limpieza = leerPositivo(
        "Costo de limpieza por viaje: "
    );

    do {
        v->kmAnuales = leerPositivo(
            "Kilometros estimados recorridos al anio: "
        );

        if (v->kmAnuales <= 0)
            printf("Los kilometros anuales deben ser mayores que cero.\n");

    } while (v->kmAnuales <= 0);
}


void crearVehiculo() {

    Vehiculo nuevo = {0};

    nuevo.id = siguienteID++;

    printf("\nNUEVO VEHICULO \n");

    registrarDatos(&nuevo);

    Vehiculo *temporal = realloc(
        vehiculos,
        (cantidad + 1) * sizeof(Vehiculo)
    );

    if (temporal == NULL) {
        printf("Error al reservar memoria.\n");
        siguienteID--;
        return;
    }

    vehiculos = temporal;
    vehiculos[cantidad] = nuevo;
    cantidad++;

    guardarVehiculos();

    printf("\nVehiculo registrado correctamente.\n");
    printf("ID asignado: %d\n", nuevo.id);
}


void mostrarVehiculos() {

    printf("\nVEHICULOS REGISTRADOS \n");

    if (cantidad == 0) {
        printf("No hay vehiculos registrados.\n");
        return;
    }

    for (int i = 0; i < cantidad; i++) {

        printf("\nID: %d", vehiculos[i].id);
        printf("\nVehiculo: %s", vehiculos[i].nombre);

        printf("\nConsumo ciudad: %.2f L/100 km",
               vehiculos[i].consumoCiudad);

        printf("\nConsumo carretera: %.2f L/100 km",
               vehiculos[i].consumoCarretera);

        printf("\nSeguro anual: RD$ %.2f",
               vehiculos[i].seguroAnual);

        printf("\nNeumaticos por km: RD$ %.2f",
               vehiculos[i].neumaticosKm);

        printf("\nMantenimiento por km: RD$ %.2f",
               vehiculos[i].mantenimientoKm);

        printf("\nValor inicial: RD$ %.2f",
               vehiculos[i].valorInicial);

        printf("\nValor final: RD$ %.2f",
               vehiculos[i].valorFinal);

        printf("\nVida util: %.2f anios",
               vehiculos[i].vidaUtil);

        printf("\nImpuestos anuales: RD$ %.2f",
               vehiculos[i].impuestosAnuales);

        printf("\nLimpieza por viaje: RD$ %.2f",
               vehiculos[i].limpieza);

        printf("\nKilometros anuales: %.2f\n",
               vehiculos[i].kmAnuales);

        printf("     \n");
    }
}



void editarVehiculo() {

    if (cantidad == 0) {
        printf("\nNo hay vehiculos para editar.\n");
        return;
    }

    mostrarVehiculos();

    int id = leerEntero("\nIntroduzca el ID a editar: ");

    int posicion = buscarVehiculo(id);

    if (posicion == -1) {
        printf("Vehiculo no encontrado.\n");
        return;
    }

    printf("\nEditando: %s\n", vehiculos[posicion].nombre);

    registrarDatos(&vehiculos[posicion]);

    guardarVehiculos();

    printf("\nVehiculo actualizado correctamente.\n");
}


void eliminarVehiculo() {

    if (cantidad == 0) {
        printf("\nNo hay vehiculos para eliminar.\n");
        return;
    }

    mostrarVehiculos();

    int id = leerEntero("\nIntroduzca el ID a eliminar: ");

    int posicion = buscarVehiculo(id);

    if (posicion == -1) {
        printf("Vehiculo no encontrado.\n");
        return;
    }

    printf("\nSe eliminara: %s\n", vehiculos[posicion].nombre);

    printf("1. Confirmar eliminacion\n");
    printf("2. Cancelar\n");

    int opcion = leerEntero("Seleccione: ");

    if (opcion != 1) {
        printf("Operacion cancelada.\n");
        return;
    }

    for (int i = posicion; i < cantidad - 1; i++) {
        vehiculos[i] = vehiculos[i + 1];
    }

    cantidad--;

    if (cantidad == 0) {
        free(vehiculos);
        vehiculos = NULL;
    } else {
        Vehiculo *temporal = realloc(
            vehiculos,
            cantidad * sizeof(Vehiculo)
        );

        if (temporal != NULL)
            vehiculos = temporal;
    }

    guardarVehiculos();

    printf("Vehiculo eliminado correctamente.\n");
}


void calcularViaje() {

    if (cantidad == 0) {
        printf("\nPrimero debe registrar un vehiculo.\n");
        return;
    }

    mostrarVehiculos();

    int id = leerEntero("\nSeleccione el ID del vehiculo: ");

    int posicion = buscarVehiculo(id);

    if (posicion == -1) {
        printf("Vehiculo no encontrado.\n");
        return;
    }

    Vehiculo v = vehiculos[posicion];

    printf("\nDATOS DEL VIAJE\n");

    double distancia = leerPositivo(
        "Distancia total del viaje (km): "
    );

    if (distancia <= 0) {
        printf("La distancia debe ser mayor que cero.\n");
        return;
    }

    double porcentajeCarretera;
    double porcentajeCiudad;

    do {
        porcentajeCarretera = leerPositivo(
            "Porcentaje en carretera: "
        );

        porcentajeCiudad = leerPositivo(
            "Porcentaje en ciudad: "
        );

        if (porcentajeCarretera > 100 ||
            porcentajeCiudad > 100 ||
            fabs(porcentajeCarretera + porcentajeCiudad - 100) > 0.000001) {

            printf("\nLos porcentajes deben sumar exactamente 100%%.\n");
        }

    } while (porcentajeCarretera > 100 ||
             porcentajeCiudad > 100 ||
             fabs(porcentajeCarretera + porcentajeCiudad - 100) > 0.000001);

    double precioCombustible = leerPositivo(
        "Precio del combustible por litro: "
    );


    double kmCiudad = distancia * porcentajeCiudad / 100.0;

    double kmCarretera = distancia * porcentajeCarretera / 100.0;


    double litrosCiudad =
        (kmCiudad * v.consumoCiudad) / 100.0;

    double litrosCarretera =
        (kmCarretera * v.consumoCarretera) / 100.0;

    double costoCombustible =
        (litrosCiudad + litrosCarretera) * precioCombustible;


    double seguroKm = v.seguroAnual / v.kmAnuales;

    double impuestosKm = v.impuestosAnuales / v.kmAnuales;


    double depreciacionKm =
        (v.valorInicial - v.valorFinal) /
        (v.vidaUtil * v.kmAnuales);

    // Costos del viaje

    double costoSeguro = seguroKm * distancia;

    double costoImpuestos = impuestosKm * distancia;

    double costoDepreciacion = depreciacionKm * distancia;

    double costoNeumaticos = v.neumaticosKm * distancia;

    double costoMantenimiento = v.mantenimientoKm * distancia;

    double costoLimpieza = v.limpieza;

    double costoTotal =
        costoCombustible +
        costoSeguro +
        costoImpuestos +
        costoDepreciacion +
        costoNeumaticos +
        costoMantenimiento +
        costoLimpieza;

    double costoKm = costoTotal / distancia;

    // RESULTADOS

    printf("\n\n   \n");
    printf("          RESULTADO DEL VIAJE\n");


    printf("Vehiculo: %s\n", v.nombre);

    printf("Distancia total: %.2f km\n", distancia);

    printf("Distancia en ciudad: %.2f km\n", kmCiudad);

    printf("Distancia en carretera: %.2f km\n", kmCarretera);

    printf("\n--------- DESGLOSE DE COSTOS ---------\n");

    printf("Combustible:       RD$ %.2f\n", costoCombustible);

    printf("Seguro:            RD$ %.2f\n", costoSeguro);

    printf("Neumaticos:        RD$ %.2f\n", costoNeumaticos);

    printf("Mantenimiento:     RD$ %.2f\n", costoMantenimiento);

    printf("Depreciacion:      RD$ %.2f\n", costoDepreciacion);

    printf("Impuestos:         RD$ %.2f\n", costoImpuestos);

    printf("Limpieza:          RD$ %.2f\n", costoLimpieza);

    printf("    \n");

    printf("COSTO TOTAL:       RD$ %.2f\n", costoTotal);

    printf("COSTO POR KM:      RD$ %.2f\n", costoKm);

    printf("    \n");
}

// ---------- MENU PRINCIPAL ----------

void mostrarMenu() {

    printf("\n   \n");
    printf("       CALCULADORA DE COSTOS DE VIAJE\n");

    printf("1. Crear vehiculo\n");
    printf("2. Mostrar vehiculos\n");
    printf("3. Editar vehiculo\n");
    printf("4. Eliminar vehiculo\n");
    printf("5. Calcular costo de viaje\n");
    printf("6. Salir\n");

    printf("   \n");
}


int main() {

    int opcion;

    cargarVehiculos();

    printf("\nBienvenido a la calculadora de viajes.\n");
    printf("Vehiculos cargados: %d\n", cantidad);

    do {

        mostrarMenu();

        opcion = leerEntero("Seleccione una opcion: ");

        switch (opcion) {

            case 1:
                crearVehiculo();
                break;

            case 2:
                mostrarVehiculos();
                break;

            case 3:
                editarVehiculo();
                break;

            case 4:
                eliminarVehiculo();
                break;

            case 5:
                calcularViaje();
                break;

            case 6:
                printf("\nGracias por utilizar el programa.\n");
                break;

            default:
                printf("\nOpcion incorrecta. Intente nuevamente.\n");
        }

    } while (opcion != 6);

    guardarVehiculos();

    free(vehiculos);
    vehiculos = NULL;

    return 0;
}

