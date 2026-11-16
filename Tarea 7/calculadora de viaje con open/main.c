   CALCULADOR DE COSTO DE VIAJE POR KILOMETRO
  Lenguaje: C (estándar, compatible con Code::Blocks / MinGW en Windows)
  Archivo: main.c

  Funcionalidades:
  - Crear, editar, eliminar y listar vehículos
  - Guardar y cargar vehículos desde archivo (vehiculos.dat)
  - Calcular costo total de un viaje y costo por kilómetro

  Parametros por vehiculo:
  1. Consumo ciudad y carretera (L/100 km)
  2. Costo seguro por año
  3. Costo neumáticos por KM
  4. Costo mantenimiento por KM
  5. Depreciacion (valor inicial, valor final, tiempo de uso)
  6. Costo impuestos por año
  7. Costo limpieza por viaje

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NOMBRE 100
#define ARCHIVO "vehiculos.dat"

/* Estructura de un vehículo */
typedef struct {
    int id;
    char nombre[MAX_NOMBRE];

    float consumo_ciudad;       /* L/100 km */
    float consumo_carretera;    /* L/100 km */

    float costo_seguro_anual;
    float costo_impuestos_anual;

    float costo_llantas_por_km;
    float costo_mantenimiento_por_km;
    float costo_limpieza_por_viaje;

    float valor_inicial;
    float valor_final;
    int tiempo_uso_anos;
    float km_por_ano;

    float depreciacion_por_km;      /* calculado */
    float costo_fijo_por_km;        /* (seguro + impuestos) / km_por_ano */
} Vehiculo;

/* Variables globales de memoria dinámica */
static Vehiculo *vehiculos = NULL;
static int num_vehiculos = 0;
static int proximo_id = 1;

  Utilidades de entrada


static void limpiar_entrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static int leer_int(const char *mensaje) {
    int valor;
    for (;;) {
        printf("%s", mensaje);
        if (scanf("%d", &valor) == 1) {
            limpiar_entrada();
            return valor;
        }
        limpiar_entrada();
        printf("Entrada invalida. Intente de nuevo.\n");
    }
}

static float leer_float(const char *mensaje) {
    float valor;
    for (;;) {
        printf("%s", mensaje);
        if (scanf("%f", &valor) == 1) {
            limpiar_entrada();
            return valor;
        }
        limpiar_entrada();
        printf("Entrada invalida. Intente de nuevo.\n");
    }
}

static void leer_string(char *dest, size_t tam, const char *mensaje) {
    printf("%s", mensaje);
    if (fgets(dest, (int)tam, stdin) != NULL) {
        size_t len = strlen(dest);
        if (len > 0 && dest[len - 1] == '\n')
            dest[len - 1] = '\0';
    } else {
        dest[0] = '\0';
        limpiar_entrada();
    }
}

/*==============================================================================
  Logica de negocio
==============================================================================*/
static void calcular_derivados(Vehiculo *v) {
    float vida_total_km = (float)v->tiempo_uso_anos * v->km_por_ano;
    if (vida_total_km > 0)
        v->depreciacion_por_km = (v->valor_inicial - v->valor_final) / vida_total_km;
    else
        v->depreciacion_por_km = 0.0f;

    if (v->km_por_ano > 0)
        v->costo_fijo_por_km = (v->costo_seguro_anual + v->costo_impuestos_anual) / v->km_por_ano;
    else
        v->costo_fijo_por_km = 0.0f;
}

static int siguiente_id(void) {
    int max_id = 0;
    for (int i = 0; i < num_vehiculos; i++) {
        if (vehiculos[i].id > max_id)
            max_id = vehiculos[i].id;
    }
    return max_id + 1;
}

/* Carga los vehículos desde archivo. Retorna la cantidad cargada. */
static int cargar_vehiculos(void) {
    FILE *arch = fopen(ARCHIVO, "r");
    if (arch == NULL)
        return 0;

    char linea[512];
    int contador = 0;

    while (fgets(linea, sizeof(linea), arch) != NULL) {
        /* Quitar salto de línea final */
        size_t len = strlen(linea);
        if (len > 0 && linea[len - 1] == '\n')
            linea[len - 1] = '\0';
        if (len == 0)
            continue;

        Vehiculo v;
        char nombre_temp[MAX_NOMBRE];
        int leidos = sscanf(linea,
            "%d|%[^|]|%f|%f|%f|%f|%f|%f|%f|%f|%f|%d|%f|%f|%f",
            &v.id, nombre_temp,
            &v.consumo_ciudad, &v.consumo_carretera,
            &v.costo_seguro_anual, &v.costo_impuestos_anual,
            &v.costo_llantas_por_km, &v.costo_mantenimiento_por_km,
            &v.costo_limpieza_por_viaje, &v.valor_inicial,
            &v.valor_final, &v.tiempo_uso_anos, &v.km_por_ano,
            &v.depreciacion_por_km, &v.costo_fijo_por_km);

        if (leidos == 15) {
            strncpy(v.nombre, nombre_temp, MAX_NOMBRE - 1);
            v.nombre[MAX_NOMBRE - 1] = '\0';

            Vehiculo *tmp = realloc(vehiculos, (contador + 1) * sizeof(Vehiculo));
            if (tmp == NULL) {
                fprintf(stderr, "Error de memoria al cargar.\n");
                fclose(arch);
                return contador;
            }
            vehiculos = tmp;
            vehiculos[contador] = v;
            contador++;
        }
    }

    fclose(arch);
    num_vehiculos = contador;
    proximo_id = siguiente_id();
    return num_vehiculos;
}

/* Guarda todos los vehículos en el archivo */
static void guardar_vehiculos(void) {
    FILE *arch = fopen(ARCHIVO, "w");
    if (arch == NULL) {
        fprintf(stderr, "No se pudo abrir el archivo para guardar.\n");
        return;
    }

    for (int i = 0; i < num_vehiculos; i++) {
        Vehiculo *v = &vehiculos[i];
        fprintf(arch,
            "%d|%s|%.4f|%.4f|%.4f|%.4f|%.4f|%.4f|%.4f|%.4f|%.4f|%d|%.4f|%.4f|%.4f\n",
            v->id, v->nombre,
            v->consumo_ciudad, v->consumo_carretera,
            v->costo_seguro_anual, v->costo_impuestos_anual,
            v->costo_llantas_por_km, v->costo_mantenimiento_por_km,
            v->costo_limpieza_por_viaje, v->valor_inicial,
            v->valor_final, v->tiempo_uso_anos, v->km_por_ano,
            v->depreciacion_por_km, v->costo_fijo_por_km);
    }

    fclose(arch);
}

/* Busca el índice de un vehículo por ID. Retorna -1 si no existe. */
static int buscar_indice_por_id(int id) {
    for (int i = 0; i < num_vehiculos; i++) {
        if (vehiculos[i].id == id)
            return i;
    }
    return -1;
}

/*==============================================================================
  Operaciones CRUD
==============================================================================*/
static void crear_vehiculo(void) {
    Vehiculo v;
    v.id = proximo_id;

    printf("\n--- CREAR VEHICULO ---\n");
    leer_string(v.nombre, sizeof(v.nombre), "Nombre del vehiculo: ");
    if (v.nombre[0] == '\0') {
        strcpy(v.nombre, "Vehiculo sin nombre");
    }

    printf("\nConsumo de combustible (litros por 100 km):\n");
    v.consumo_ciudad = leer_float("  Consumo en ciudad (L/100km): ");
    v.consumo_carretera = leer_float("  Consumo en carretera (L/100km): ");

    printf("\nCostos anuales fijos:\n");
    v.costo_seguro_anual = leer_float("  Costo del seguro por año: ");
    v.costo_impuestos_anual = leer_float("  Costo de impuestos por año: ");

    printf("\nCostos por kilómetro:\n");
    v.costo_llantas_por_km = leer_float("  Costo de neumáticos por KM: ");
    v.costo_mantenimiento_por_km = leer_float("  Costo de mantenimiento por KM: ");
    v.costo_limpieza_por_viaje = leer_float("  Costo de limpieza por viaje: ");

    printf("\nDepreciación:\n");
    v.valor_inicial = leer_float("  Valor inicial del vehículo: ");
    v.valor_final = leer_float("  Valor final (residual) del vehículo: ");
    v.tiempo_uso_anos = leer_int("  Tiempo de uso (años): ");
    v.km_por_ano = leer_float("  Kilómetros estimados por año: ");

    calcular_derivados(&v);

    /* Agregar al arreglo dinámico */
    Vehiculo *tmp = realloc(vehiculos, (num_vehiculos + 1) * sizeof(Vehiculo));
    if (tmp == NULL) {
        fprintf(stderr, "Error de memoria al crear vehículo.\n");
        return;
    }
    vehiculos = tmp;
    vehiculos[num_vehiculos] = v;
    num_vehiculos++;
    proximo_id++;

    guardar_vehiculos();
    printf("\nVehiculo '%s' creado con ID %d.\n", v.nombre, v.id);
}

static void listar_vehiculos(void) {
    if (num_vehiculos == 0) {
        printf("\nNo hay vehículos registrados.\n");
        return;
    }

    printf("\n--- LISTA DE VEHICULOS ---\n");
    printf("%-5s %-20s %-12s %-12s %-12s\n",
        "ID", "Nombre", "Ciudad L/100", "Carretera L/100", "Deprec./km");
    printf("------------------------------------------------------------\n");
    for (int i = 0; i < num_vehiculos; i++) {
        Vehiculo *v = &vehiculos[i];
        printf("%-5d %-20s %-12.2f %-12.2f %-12.4f\n",
            v->id, v->nombre, v->consumo_ciudad,
            v->consumo_carretera, v->depreciacion_por_km);
    }
}

static void editar_vehiculo(void) {
    if (num_vehiculos == 0) {
        printf("\nNo hay vehículos registrados.\n");
        return;
    }

    listar_vehiculos();
    int id = leer_int("\nID del vehículo a editar: ");
    int idx = buscar_indice_por_id(id);
    if (idx == -1) {
        printf("No se encontró un vehículo con ese ID.\n");
        return;
    }

    Vehiculo *v = &vehiculos[idx];

    printf("\nEditando: %s (ID %d)\n", v->nombre, v->id);
    printf("(Ingrese los nuevos valores; se sobrescribirán los actuales)\n");

    leer_string(v->nombre, sizeof(v->nombre), "Nombre: ");
    if (v->nombre[0] == '\0')
        strcpy(v->nombre, "Vehiculo sin nombre");

    printf("\nConsumo de combustible (L/100 km):\n");
    v->consumo_ciudad = leer_float("  Consumo en ciudad: ");
    v->consumo_carretera = leer_float("  Consumo en carretera: ");

    printf("\nCostos anuales fijos:\n");
    v->costo_seguro_anual = leer_float("  Seguro por año: ");
    v->costo_impuestos_anual = leer_float("  Impuestos por año: ");

    printf("\nCostos por kilómetro:\n");
    v->costo_llantas_por_km = leer_float("  Neumáticos por KM: ");
    v->costo_mantenimiento_por_km = leer_float("  Mantenimiento por KM: ");
    v->costo_limpieza_por_viaje = leer_float("  Limpieza por viaje: ");

    printf("\nDepreciación:\n");
    v->valor_inicial = leer_float("  Valor inicial: ");
    v->valor_final = leer_float("  Valor final: ");
    v->tiempo_uso_anos = leer_int("  Tiempo de uso (años): ");
    v->km_por_ano = leer_float("  Kilómetros por año: ");

    calcular_derivados(v);
    guardar_vehiculos();
    printf("\nVehículo '%s' actualizado.\n", v->nombre);
}

static void eliminar_vehiculo(void) {
    if (num_vehiculos == 0) {
        printf("\nNo hay vehículos registrados.\n");
        return;
    }

    listar_vehiculos();
    int id = leer_int("\nID del vehículo a eliminar: ");
    int idx = buscar_indice_por_id(id);
    if (idx == -1) {
        printf("No se encontró un vehículo con ese ID.\n");
        return;
    }

    char nombre_borrado[MAX_NOMBRE];
    strcpy(nombre_borrado, vehiculos[idx].nombre);

    char confirmar;
    printf("¿Está seguro que desea eliminar '%s'? (s/n): ", nombre_borrado);
    scanf(" %c", &confirmar);
    limpiar_entrada();

    if (confirmar == 's' || confirmar == 'S') {
        /* Desplazar elementos para eliminar */
        for (int i = idx; i < num_vehiculos - 1; i++) {
            vehiculos[i] = vehiculos[i + 1];
        }
        num_vehiculos--;

        if (num_vehiculos > 0) {
            Vehiculo *tmp = realloc(vehiculos, num_vehiculos * sizeof(Vehiculo));
            if (tmp != NULL)
                vehiculos = tmp;
        } else {
            free(vehiculos);
            vehiculos = NULL;
        }

        guardar_vehiculos();
        printf("Vehículo '%s' eliminado.\n", nombre_borrado);
    } else {
        printf("Operación cancelada.\n");
    }
}

/*==============================================================================
  Calculo de viaje
==============================================================================*/
static void calcular_viaje(void) {
    if (num_vehiculos == 0) {
        printf("\nNo hay vehículos registrados. Cree uno primero.\n");
        return;
    }

    listar_vehiculos();
    int id = leer_int("\nSeleccione el ID del vehículo: ");
    int idx = buscar_indice_por_id(id);
    if (idx == -1) {
        printf("No se encontró el vehículo.\n");
        return;
    }

    Vehiculo *v = &vehiculos[idx];

    float distancia = leer_float("Distancia total del viaje (km): ");
    if (distancia <= 0) {
        printf("La distancia debe ser mayor que cero.\n");
        return;
    }

    float pct_carretera = leer_float("Porcentaje en carretera (%): ");
    float pct_ciudad = 100.0f - pct_carretera;
    if (pct_ciudad < 0.0f) {
        printf("El porcentaje en carretera no puede exceder el 100%%.\n");
        return;
    }

    float precio_combustible = leer_float("Precio del combustible por litro: ");
    if (precio_combustible < 0) {
        printf("El precio no puede ser negativo.\n");
        return;
    }

    float km_carretera = distancia * pct_carretera / 100.0f;
    float km_ciudad = distancia * pct_ciudad / 100.0f;

    float litros_carretera = km_carretera * v->consumo_carretera / 100.0f;
    float litros_ciudad = km_ciudad * v->consumo_ciudad / 100.0f;
    float total_litros = litros_carretera + litros_ciudad;
    float costo_combustible = total_litros * precio_combustible;

    costo_combustible = total_litros * precio_combustible;
    float costo_llantas = distancia * v->costo_llantas_por_km;
    float costo_mantenimiento = distancia * v->costo_mantenimiento_por_km;
    float costo_limpieza = v->costo_limpieza_por_viaje;
    float costo_fijos_viaje = distancia * v->costo_fijo_por_km;
    float depreciacion_viaje = distancia * v->depreciacion_por_km;

    float total = costo_combustible + costo_llantas + costo_mantenimiento +
                  costo_limpieza + costo_fijos_viaje + depreciacion_viaje;
    float costo_por_km = total / distancia;

    printf("\n============================================\n");
    printf("VEHICULO: %s\n", v->nombre);
    printf("Distancia: %.2f km\n", distancia);
    printf("  Carretera: %.2f km (%.0f%%)\n", km_carretera, pct_carretera);
    printf("  Ciudad:    %.2f km (%.0f%%)\n", km_ciudad, pct_ciudad);
    printf("--------------------------------------------\n");
    printf("Combustible (%.2f L x %.2f): %.2f\n", total_litros, precio_combustible, costo_combustible);
    printf("Neumaticos:                         %.2f\n", costo_llantas);
    printf("Mantenimiento:                      %.2f\n", costo_mantenimiento);
    printf("Limpieza (por viaje):               %.2f\n", costo_limpieza);
    printf("Seguro + Impuestos (prorrateados):  %.2f\n", costo_fijos_viaje);
    printf("Depreciacion:                       %.2f\n", depreciacion_viaje);
    printf("============================================\n");
    printf("COSTO TOTAL DEL VIAJE:      %.2f\n", total);
    printf("COSTO POR KILOMETRO:        %.4f\n", costo_por_km);
    printf("============================================\n");
}

/*==============================================================================
  Menu principal
==============================================================================*/
static void mostrar_menu(void) {
    printf("\n============================================\n");
    printf(" CALCULADOR DE COSTO DE VIAJE POR KM\n");
    printf("============================================\n");
    printf(" 1. Crear vehículo\n");
    printf(" 2. Editar vehículo\n");
    printf(" 3. Eliminar vehículo\n");
    printf(" 4. Listar vehículos\n");
    printf(" 5. Calcular costo de viaje\n");
    printf(" 6. Salir\n");
    printf("--------------------------------------------\n");
}

int main(void) {
    cargar_vehiculos();

    int opcion;
    for (;;) {
        mostrar_menu();
        opcion = leer_int("Seleccione una opción: ");

        switch (opcion) {
            case 1:
                crear_vehiculo();
                break;
            case 2:
                editar_vehiculo();
                break;
            case 3:
                eliminar_vehiculo();
                break;
            case 4:
                listar_vehiculos();
                break;
            case 5:
                calcular_viaje();
                break;
            case 6:
                printf("\n¡Hasta luego!\n");
                free(vehiculos);
                return 0;
            default:
                printf("Opción no válida. Intente de nuevo.\n");
        }
    }

    /* No se llega aquí normalmente, pero por seguridad: */
    free(vehiculos);
    return 0;
}
