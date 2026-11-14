#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "validaciones.h"

// Limpia la terminal
void limpiar_pantalla(void) {
    system("cls");
}

// Pausa la ejecucion hasta presionar enter
void pausar_pantalla(void) {
    printf("\nPresione ENTER para continuar...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        // vacia cualquier caracter que haya quedado
    }
}

// Limpia el buffer de entrada hasta el salto de linea
void limpiar_linea(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        // descarta caracteres sobrantes
    }
}

// Lee un entero y valida que este dentro del rango permitido
int leer_entero(const char *msg, int min, int max) {
    int n, r, c;
    while (1) {
        printf("%s", msg);
        r = scanf("%d", &n);
        if (r == EOF) {
            exit(1);
        }
        if (r == 1) {
            c = getchar();
            if (c == '\n') {
                if (n >= min && n <= max) {
                    return n;
                }
                limpiar_pantalla();
                printf("Error: Fuera de rango (%d a %d).\n\n", min, max);
                continue;
            }
            if (c != EOF) {
                limpiar_linea();
            }
        } else {
            limpiar_linea();
        }
        limpiar_pantalla();
        printf("Error: Entrada invalida. Ingrese un numero entero.\n\n");
    }
}

// Lee un double, admite tanto punto como coma decimal y valida el rango
double leer_double(const char *msg, double min, double max) {
    char cad[64];
    double x;
    int r;
    while (1) {
        printf("%s", msg);
        if (scanf("%63s", cad) != 1) {
            limpiar_linea();
            continue;
        }
        limpiar_linea();

        // Si el usuario uso coma como separador decimal, la cambiamos a punto
        for (int i = 0; cad[i] != '\0'; i++) {
            if (cad[i] == ',') {
                cad[i] = '.';
            }
        }

        r = sscanf(cad, "%lf", &x);
        if (r == 1) {
            if (x >= min && x <= max) {
                return x;
            }
            limpiar_pantalla();
            printf("Error: Fuera de rango (%.2f a %.2f).\n\n", min, max);
            continue;
        }
        limpiar_pantalla();
        printf("Error: Entrada invalida. Ingrese un valor numerico.\n\n");
    }
}

// Solicita confirmacion s/n para acciones importantes
int leer_confirmacion(const char *msg) {
    char resp;
    while (1) {
        printf("%s (s/n): ", msg);
        if (scanf(" %c", &resp) == 1) {
            limpiar_linea();
            if (resp == 's' || resp == 'S') {
                return 1;
            }
            if (resp == 'n' || resp == 'N') {
                return 0;
            }
        } else {
            limpiar_linea();
        }
        printf("Error: Ingrese 's' para si o 'n' para no.\n\n");
    }
}
