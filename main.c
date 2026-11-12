#include <stdio.h>
#include "validaciones.h"
#include <windows.h>
#include "logica_matematica.h"
typedef struct res {
    char nombre[30];
    double valor;
    char unidad[10];
} resultado_st;
typedef struct mision {
    double alt;
    double masa;
    double radio;
    double veloc;
    double periodo;
    double acel_cent; //aceleración centrípeta
    double fuerza;
    int cargada;
} mision_st;
resultado_st resultados[50]; //maxima cantidad de resultados posibles
int n_resultados = 0; // cuantos arreglos están usados
void registrar(resultado_st res[], int* n, const char* nombre, double valor, const char* unidad) {
    if (*n >= 50) {
        printf("Reporte lleno: No se guardó el resultado- \n");
        return;
        strcpy(res[*n].nombre, nombre);
        res[*n].valor = valor;
        strcpy(res[*n].unidad, unidad);
        (*n)++;
    }
}
//en la anterior función paso como parámetro un puntero pq es una forma de no hacer una copia de la variable, sino incrementar la original para que main vea el cambio, con (*n)++ incremento el valor, con *n++ estoy incrementando la dirección de memoria, pero no el valor.
void mostrar_reporte(resultado_st res[], int n) {
    int i;
    if n == 0 {
        printf("Aun no hay resultados registrados.\n");
        return;
    }
    printf("===========REPORTE===========\n");
    for (i = 0; i < n; i++) {
        printf("Resultado %d: %s = %f %s\n", i + 1, res[i].nombre, res[i].valor, res[i].unidad);
    }
    printf("Total de cálculos: %d\n", n);
}
void mostrar_menu() {
    printf("=================================\n");
    printf("      SIMULADOR ARTEMIS      \n");
    printf("=================================\n");
    printf("1- Velocidad Orbital de la Nave\n");
    printf("2- Período Orbital\n");
    printf("3- Aceleración Centrípeta\n");
    printf("4- Pulso Lateral\n");
    printf("5- Ver Reporte\n");
    printf("6- Cambiar Altitud/Masa\n");
    printf("7- Salir\n");
    printf("---------------------------------\n");
}
void cargar_mision(mision_st *m) {
    printf("=================================\n");
    printf("      CARGA DE DATOS ARTEMIS      \n");
    printf("=================================\n");
    printf("Ingrese la altitud en km (160 a 2000) a la que Orion se desplaza :\n");
    m->alt = leer_double("Altitud en km (160 a 2000): ", 160, 2000) * 1000;
    printf("Ingrese la masa de la nave en kg (1000 a 100.000)");
    m->masa = leer_double("Masa en kg (1000 a 100000)", 1000, 100000);
    m->cargada = 1; //por si el usuario intenta calcular las opciones sin haber ingresado datos
    calcular_orbita(&mision);
    printf("Misión cargada h = %.1f km | m = %.1f kg\n", m->alt / 1000.0, m->masa);
}
int main() {
    SetConsoleOutputCP(CP_UTF8);
    mision_st mision = {0};
    int opc = 0;
    cargar_mision(&mision);
    do {
        mostrar_menu();
        opc = leer_entero("Elija una opción (1-7)", 1, 7);
        switch (opc) {
            case 1: 
                printf("Elegiste calcular la Velocidad Orbital\n");
                printf("Velocidad Orbital: %.2f m/s\n", mision.veloc);
                registrar(resultados, &n_resultados, "Velocidad Orbital", mision.veloc, "m/s");
                break;
            case 2: 
                printf("Elegiste calcular el Periodo Orbital\n");
                registrar(resultados, &n_resultados, "Periodo orbital", mision.periodo, "s");
                break;
            case 3:
                printf("Elegiste calcular la Aceleración Centrípeta\n");
                printf("Aceleración Centrípeta: %.4f m/s^2\n", mision.acel_cent);
                registrar(resultados, &n_resultados, "Aceleración centrípeta", mision.acel_cent, "m/s^2");
                break;
            case 4:
                printf("Elegiste calcular el Pulso Lateral\n");
                a = leer_double("Aceleración del pulso en m/s^2 (0.1 a 5): ", 0.1, 5);
                dt = leer_double("Duración en s (1 a 60): ", 1, 60);
                calcular_pulso(a, dt, &v_lat, &desvio);
                printf("Velocidad Lateral: %.2f m/s\n, v_lat");
                printf("Desvio Lateral: %.2f m\n", desvio);
                registrar(resultados, &n_resultados, "Velocidad Lateral", v_lat, "m/s");
                registrar(resultados, &n_resultados, "Desvio Lateral", desvio, "m");
                break;
            case 5:
                printf("Elegiste ver reporte\n");
                mostrar_reporte(resultados, n_resultados);
                break;
            case 6:
                printf("Elegiste Cambiar Altitud/Masa");
                cargar_mision(&mision);
                break;
            case 7: 
                printf("Elegiste salir\n");
                print("Adios.")
                break;
        }
    } while (opc != 7);
    return 0;
}
