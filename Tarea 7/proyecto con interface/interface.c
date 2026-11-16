

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define ARCHIVO "vehiculos.dat"
#define MAX_CAMPOS 12
#define MAX_VIAJE 4

#define ID_LISTA 100
#define ID_CREAR 101
#define ID_EDITAR 102
#define ID_ELIMINAR 103
#define ID_LIMPIAR 104
#define ID_CALCULAR 105

HWND ventana;
HWND campos[MAX_CAMPOS];
HWND viaje[MAX_VIAJE];
HWND lista;

Vehiculo_placeholder;

// Estructura del vehiculo
typedef struct {
    char nombre[50];
    double ciudad, carretera;
    double seguro, neumaticos, mantenimiento;
    double inicial, final, vida;
    double impuestos, limpieza, kmAnuales;
} Vehiculo;

Vehiculo *vehiculos = NULL;
int cantidad = 0;

// Identificadores de los campos
const char *nombres[MAX_CAMPOS] = {
    "Nombre / modelo",
    "Consumo ciudad (L/100km)",
    "Consumo carretera (L/100km)",
    "Seguro anual",
    "Neumaticos por km",
    "Mantenimiento por km",
    "Valor inicial",
    "Valor final",
    "Vida util (anios)",
    "Impuestos anuales",
    "Limpieza por viaje",
    "Km recorridos al anio"
};

const char *nombresViaje[MAX_VIAJE] = {
    "Distancia total (km)",
    "Porcentaje carretera",
    "Porcentaje ciudad",
    "Precio combustible / litro"
};

// Crear etiquetas
void etiqueta(HWND padre, const char *texto,
              int x, int y, int ancho, int alto) {

    CreateWindowA("STATIC", texto,
        WS_CHILD | WS_VISIBLE,
        x, y, ancho, alto,
        padre, NULL, NULL, NULL);
}

// Crear campos de texto
HWND crearCampo(HWND padre, int x, int y,
                int ancho, int alto) {

    return CreateWindowExA(
        WS_EX_CLIENTEDGE,
        "EDIT", "",
        WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        x, y, ancho, alto,
        padre, NULL, NULL, NULL
    );
}

// Obtener texto de un campo
void obtenerTexto(HWND campo, char *texto, int tamano) {
    GetWindowTextA(campo, texto, tamano);
}

// Convertir campo a numero
int obtenerNumero(HWND campo, double *numero) {

    char texto[100];
    char *fin;

    obtenerTexto(campo, texto, sizeof(texto));

    *numero = strtod(texto, &fin);

    while (*fin == ' ' || *fin == '\t')
        fin++;

    if (texto[0] == '\0' || *fin != '\0' ||
        !isfinite(*numero) || *numero < 0) {
        return 0;
    }

    return 1;
}

// Guardar vehiculos
void guardar() {

    FILE *archivo = fopen(ARCHIVO, "wb");

    if (!archivo) {
        MessageBoxA(ventana, "No se pudo guardar el archivo.",
                    "Error", MB_ICONERROR);
        return;
    }

    fwrite(&cantidad, sizeof(int), 1, archivo);

    if (cantidad > 0)
        fwrite(vehiculos, sizeof(Vehiculo), cantidad, archivo);

    fclose(archivo);
}

// Cargar vehiculos
void cargar() {

    FILE *archivo = fopen(ARCHIVO, "rb");

    if (!archivo) return;

    int total;

    if (fread(&total, sizeof(int), 1, archivo) != 1 ||
        total < 0 || total > 10000) {
        fclose(archivo);
        return;
    }

    if (total > 0) {

        vehiculos = malloc(total * sizeof(Vehiculo));

        if (!vehiculos) {
            fclose(archivo);
            return;
        }

        cantidad = (int)fread(
            vehiculos, sizeof(Vehiculo), total, archivo
        );

        if (cantidad != total) {
            free(vehiculos);
            vehiculos = NULL;
            cantidad = 0;
        }
    }

    fclose(archivo);
}

// Actualizar lista visual
void actualizarLista() {

    SendMessageA(lista, LB_RESETCONTENT, 0, 0);

    char texto[150];

    for (int i = 0; i < cantidad; i++) {

        sprintf(texto, "%s", vehiculos[i].nombre);

        SendMessageA(lista, LB_ADDSTRING, 0,
                     (LPARAM)texto);
    }
}

// Limpiar formulario
void limpiarCampos() {

    for (int i = 0; i < MAX_CAMPOS; i++)
        SetWindowTextA(campos[i], "");

}

// Cargar datos de un vehiculo seleccionado
void seleccionarVehiculo() {

    int indice = (int)SendMessageA(
        lista, LB_GETCURSEL, 0, 0
    );

    if (indice < 0 || indice >= cantidad)
        return;

    Vehiculo v = vehiculos[indice];

    char texto[100];

    SetWindowTextA(campos[0], v.nombre);

    double valores[MAX_CAMPOS - 1] = {
        v.ciudad, v.carretera, v.seguro,
        v.neumaticos, v.mantenimiento,
        v.inicial, v.final, v.vida,
        v.impuestos, v.limpieza, v.kmAnuales
    };

    for (int i = 1; i < MAX_CAMPOS; i++) {
        sprintf(texto, "%.4f", valores[i - 1]);
        SetWindowTextA(campos[i], texto);
    }
}

// Leer formulario del vehiculo
int leerVehiculo(Vehiculo *v) {

    obtenerTexto(campos[0], v->nombre, sizeof(v->nombre));

    if (strlen(v->nombre) == 0) {
        MessageBoxA(ventana, "Introduzca el nombre del vehiculo.",
                    "Aviso", MB_ICONWARNING);
        return 0;
    }

    double *valores[MAX_CAMPOS - 1] = {
        &v->ciudad, &v->carretera, &v->seguro,
        &v->neumaticos, &v->mantenimiento,
        &v->inicial, &v->final, &v->vida,
        &v->impuestos, &v->limpieza, &v->kmAnuales
    };

    for (int i = 1; i < MAX_CAMPOS; i++) {

        if (!obtenerNumero(campos[i], valores[i - 1])) {

            char mensaje[150];

            sprintf(mensaje, "Revise el campo: %s", nombres[i]);

            MessageBoxA(ventana, mensaje,
                        "Dato incorrecto", MB_ICONWARNING);
            return 0;
        }
    }

    if (v->vida <= 0 || v->kmAnuales <= 0) {
        MessageBoxA(ventana,
            "La vida util y los kilometros anuales deben ser mayores que cero.",
            "Dato incorrecto", MB_ICONWARNING);
        return 0;
    }

    if (v->final > v->inicial) {
        MessageBoxA(ventana,
            "El valor final no puede superar el valor inicial.",
            "Dato incorrecto", MB_ICONWARNING);
        return 0;
    }

    return 1;
}

// Crear vehiculo
void crearVehiculo() {

    Vehiculo nuevo;

    if (!leerVehiculo(&nuevo))
        return;

    Vehiculo *temporal = realloc(
        vehiculos, (cantidad + 1) * sizeof(Vehiculo)
    );

    if (!temporal) {
        MessageBoxA(ventana, "No hay memoria disponible.",
                    "Error", MB_ICONERROR);
        return;
    }

    vehiculos = temporal;
    vehiculos[cantidad] = nuevo;
    cantidad++;

    guardar();
    actualizarLista();
    limpiarCampos();

    MessageBoxA(ventana, "Vehiculo registrado correctamente.",
                "Correcto", MB_ICONINFORMATION);
}

// Editar vehiculo
void editarVehiculo() {

    int indice = (int)SendMessageA(
        lista, LB_GETCURSEL, 0, 0
    );

    if (indice < 0 || indice >= cantidad) {
        MessageBoxA(ventana, "Seleccione un vehiculo.",
                    "Aviso", MB_ICONWARNING);
        return;
    }

    Vehiculo nuevo;

    if (!leerVehiculo(&nuevo))
        return;

    vehiculos[indice] = nuevo;

    guardar();
    actualizarLista();

    SendMessageA(lista, LB_SETCURSEL, indice, 0);

    MessageBoxA(ventana, "Vehiculo actualizado.",
                "Correcto", MB_ICONINFORMATION);
}

// Eliminar vehiculo
void eliminarVehiculo() {

    int indice = (int)SendMessageA(
        lista, LB_GETCURSEL, 0, 0
    );

    if (indice < 0 || indice >= cantidad) {
        MessageBoxA(ventana, "Seleccione un vehiculo.",
                    "Aviso", MB_ICONWARNING);
        return;
    }

    if (MessageBoxA(ventana,
        "Desea eliminar este vehiculo?",
        "Confirmar", MB_YESNO | MB_ICONQUESTION) != IDYES)
        return;

    for (int i = indice; i < cantidad - 1; i++)
        vehiculos[i] = vehiculos[i + 1];

    cantidad--;

    if (cantidad == 0) {
        free(vehiculos);
        vehiculos = NULL;
    } else {
        Vehiculo *temporal = realloc(
            vehiculos, cantidad * sizeof(Vehiculo)
        );

        if (temporal)
            vehiculos = temporal;
    }

    guardar();
    actualizarLista();
    limpiarCampos();
}

// Calcular viaje
void calcularViaje() {

    int indice = (int)SendMessageA(
        lista, LB_GETCURSEL, 0, 0
    );

    if (indice < 0 || indice >= cantidad) {
        MessageBoxA(ventana, "Seleccione un vehiculo.",
                    "Aviso", MB_ICONWARNING);
        return;
    }

    double distancia, carretera, ciudad, precio;

    double *datos[MAX_VIAJE] = {
        &distancia, &carretera, &ciudad, &precio
    };

    for (int i = 0; i < MAX_VIAJE; i++) {

        if (!obtenerNumero(viaje[i], datos[i])) {
            MessageBoxA(ventana,
                "Complete correctamente todos los datos del viaje.",
                "Error", MB_ICONWARNING);
            return;
        }
    }

    if (distancia <= 0 || carretera > 100 ||
        ciudad > 100 || fabs(carretera + ciudad - 100) > 0.000001) {

        MessageBoxA(ventana,
            "La distancia debe ser positiva y los porcentajes deben sumar 100%.",
            "Datos incorrectos", MB_ICONWARNING);
        return;
    }

    Vehiculo v = vehiculos[indice];

    double kmCiudad = distancia * ciudad / 100.0;
    double kmCarretera = distancia * carretera / 100.0;

    double litros =
        (kmCiudad * v.ciudad +
         kmCarretera * v.carretera) / 100.0;

    double combustible = litros * precio;

    double seguro = (v.seguro / v.kmAnuales) * distancia;

    double impuestos =
        (v.impuestos / v.kmAnuales) * distancia;

    double depreciacion =
        ((v.inicial - v.final) /
        (v.vida * v.kmAnuales)) * distancia;

    double neumaticos = v.neumaticos * distancia;

    double mantenimiento = v.mantenimiento * distancia;

    double limpieza = v.limpieza;

    double total = combustible + seguro + impuestos +
        depreciacion + neumaticos + mantenimiento + limpieza;

    double porKm = total / distancia;

    char resultado[1800];

    snprintf(resultado, sizeof(resultado),
        "VEHICULO: %s\n"
        "DISTANCIA: %.2f km\n\n"
        "DESGLOSE DE COSTOS:\n"
        "Combustible:       RD$ %.2f\n"
        "Seguro:            RD$ %.2f\n"
        "Neumaticos:        RD$ %.2f\n"
        "Mantenimiento:     RD$ %.2f\n"
        "Depreciacion:      RD$ %.2f\n"
        "Impuestos:         RD$ %.2f\n"
        "Limpieza:          RD$ %.2f\n"
        "--------------------------------\n"
        "COSTO TOTAL:       RD$ %.2f\n"
        "COSTO POR KM:      RD$ %.2f",
        v.nombre, distancia,
        combustible, seguro, neumaticos,
        mantenimiento, depreciacion, impuestos,
        limpieza, total, porKm
    );

    MessageBoxA(ventana, resultado,
                "Resultado del viaje", MB_OK | MB_ICONINFORMATION);
}

// Crear interfaz
void crearInterfaz(HWND hwnd) {

    etiqueta(hwnd, "CALCULADORA DE COSTOS DE VIAJES",
             20, 15, 700, 35);

    etiqueta(hwnd, "DATOS DEL VEHICULO",
             20, 55, 300, 25);

    for (int i = 0; i < MAX_CAMPOS; i++) {

        int columna = i / 6;
        int fila = i % 6;

        int x = 20 + columna * 265;
        int y = 90 + fila * 55;

        etiqueta(hwnd, nombres[i], x, y, 250, 20);

        campos[i] = crearCampo(
            hwnd, x, y + 22, 235, 25
        );
    }

    etiqueta(hwnd, "VEHICULOS REGISTRADOS",
             560, 55, 300, 25);

    lista = CreateWindowA(
        "LISTBOX", "",
        WS_CHILD | WS_VISIBLE | WS_BORDER |
        LBS_NOTIFY | WS_VSCROLL,
        560, 90, 430, 300,
        hwnd, (HMENU)ID_LISTA, NULL, NULL
    );

    CreateWindowA("BUTTON", "Crear",
        WS_CHILD | WS_VISIBLE,
        560, 405, 100, 35,
        hwnd, (HMENU)ID_CREAR, NULL, NULL);

    CreateWindowA("BUTTON", "Editar",
        WS_CHILD | WS_VISIBLE,
        670, 405, 100, 35,
        hwnd, (HMENU)ID_EDITAR, NULL, NULL);

    CreateWindowA("BUTTON", "Eliminar",
        WS_CHILD | WS_VISIBLE,
        780, 405, 100, 35,
        hwnd, (HMENU)ID_ELIMINAR, NULL, NULL);

    CreateWindowA("BUTTON", "Limpiar",
        WS_CHILD | WS_VISIBLE,
        890, 405, 100, 35,
        hwnd, (HMENU)ID_LIMPIAR, NULL, NULL);

    etiqueta(hwnd, "DATOS DEL VIAJE",
             20, 465, 300, 25);

    for (int i = 0; i < MAX_VIAJE; i++) {

        int x = 20 + i * 245;

        etiqueta(hwnd, nombresViaje[i],
                 x, 505, 230, 20);

        viaje[i] = crearCampo(
            hwnd, x, 530, 210, 28
        );
    }

    CreateWindowA("BUTTON", "CALCULAR VIAJE",
        WS_CHILD | WS_VISIBLE,
        20, 590, 250, 45,
        hwnd, (HMENU)ID_CALCULAR, NULL, NULL);

    actualizarLista();
}

// Procesar eventos
LRESULT CALLBACK procedimiento(
    HWND hwnd, UINT mensaje,
    WPARAM wParam, LPARAM lParam) {

    switch (mensaje) {

        case WM_CREATE:
            crearInterfaz(hwnd);
            return 0;

        case WM_COMMAND:

            switch (LOWORD(wParam)) {

                case ID_CREAR:
                    crearVehiculo();
                    break;

                case ID_EDITAR:
                    editarVehiculo();
                    break;

                case ID_ELIMINAR:
                    eliminarVehiculo();
                    break;

                case ID_LIMPIAR:
                    limpiarCampos();
                    break;

                case ID_CALCULAR:
                    calcularViaje();
                    break;

                case ID_LISTA:
                    if (HIWORD(wParam) == LBN_SELCHANGE)
                        seleccionarVehiculo();
                    break;
            }

            return 0;

        case WM_DESTROY:
            guardar();
            free(vehiculos);
            vehiculos = NULL;
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcA(hwnd, mensaje, wParam, lParam);
}

// Inicio del programa
int WINAPI WinMain(
    HINSTANCE instancia,
    HINSTANCE anterior,
    LPSTR argumentos,
    int mostrar) {

    (void)anterior;
    (void)argumentos;

    cargar();

    WNDCLASSA clase = {0};

    clase.lpfnWndProc = procedimiento;
    clase.hInstance = instancia;
    clase.lpszClassName = "CalculadoraViajes";
    clase.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    clase.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClassA(&clase)) {
        free(vehiculos);
        return 1;
    }

    ventana = CreateWindowA(
        "CalculadoraViajes",
        "Calculadora de Costos de Viajes",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        1030, 720,
        NULL, NULL, instancia, NULL
    );

    if (!ventana) {
        free(vehiculos);
        return 1;
    }

    ShowWindow(ventana, mostrar);
    UpdateWindow(ventana);

    MSG mensaje;

    while (GetMessage(&mensaje, NULL, 0, 0) > 0) {
        TranslateMessage(&mensaje);
        DispatchMessage(&mensaje);
    }

    return (int)mensaje.wParam;
}
