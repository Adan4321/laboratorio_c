#include "validaciones.h"

#include <ctype.h>
#include <errno.h>
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int leer_linea(char *buffer, size_t tam) {
    if (fgets(buffer, (int)tam, stdin) == NULL) {
        return 0;
    }

    size_t largo = strlen(buffer);

    if (largo > 0 && buffer[largo - 1] == '\n') {
        buffer[largo - 1] = '\0';
        return 1;
    }

    int c;
    int sobra = 0;

    while ((c = getchar()) != '\n' && c != EOF) {
        sobra = 1;
    }

    return sobra ? -1 : 1;
}

static void salir_por_eof(void) {
    printf("\nEntrada finalizada. Cerrando el simulador.\n");
    exit(EXIT_SUCCESS);
}

static void normalizar_separador_decimal(char *buffer) {
    struct lconv *config = localeconv();
    char separador = '.';

    if (config != NULL && config->decimal_point != NULL && config->decimal_point[0] != '\0') {
        separador = config->decimal_point[0];
    }

    for (char *p = buffer; *p != '\0'; p++) {
        if (*p == '.' || *p == ',') {
            *p = separador;
        }
    }
}

double leer_double(const char *mensaje, double minimo, double maximo) {
    char buffer[128];
    char *fin;
    double valor;

    for (;;) {
        printf("%s", mensaje);
        fflush(stdout);

        int estado = leer_linea(buffer, sizeof buffer);

        if (estado == 0) {
            salir_por_eof();
        }

        if (estado < 0) {
            printf("Error: la entrada supera el largo permitido.\n");
            continue;
        }

        normalizar_separador_decimal(buffer);

        errno = 0;
        valor = strtod(buffer, &fin);

        while (isspace((unsigned char)*fin)) {
            fin++;
        }

        if (fin == buffer || *fin != '\0' || errno == ERANGE || !isfinite(valor)) {
            printf("Error: ingrese un numero valido.\n");
            continue;
        }

        if (valor < minimo || valor > maximo) {
            printf("Error: el valor debe estar entre %.4g y %.4g.\n", minimo, maximo);
            continue;
        }

        return valor;
    }
}

int leer_entero(const char *mensaje, int minimo, int maximo) {
    char buffer[128];
    char *fin;
    long valor;

    for (;;) {
        printf("%s", mensaje);
        fflush(stdout);

        int estado = leer_linea(buffer, sizeof buffer);

        if (estado == 0) {
            salir_por_eof();
        }

        if (estado < 0) {
            printf("Error: la entrada supera el largo permitido.\n");
            continue;
        }

        errno = 0;
        valor = strtol(buffer, &fin, 10);

        while (isspace((unsigned char)*fin)) {
            fin++;
        }

        if (fin == buffer || *fin != '\0' || errno == ERANGE) {
            printf("Error: ingrese un numero entero valido.\n");
            continue;
        }

        if (valor < minimo || valor > maximo) {
            printf("Error: el valor debe estar entre %d y %d.\n", minimo, maximo);
            continue;
        }

        return (int)valor;
    }
}

int leer_confirmacion(const char *mensaje) {
    char buffer[16];

    for (;;) {
        printf("%s (s/n): ", mensaje);
        fflush(stdout);

        int estado = leer_linea(buffer, sizeof buffer);

        if (estado == 0) {
            salir_por_eof();
        }

        if (estado < 0) {
            printf("Error: la entrada supera el largo permitido.\n");
            continue;
        }

        if ((buffer[0] == 's' || buffer[0] == 'S') && buffer[1] == '\0') {
            return 1;
        }

        if ((buffer[0] == 'n' || buffer[0] == 'N') && buffer[1] == '\0') {
            return 0;
        }

        printf("Error: responda 's' o 'n'.\n");
    }
}

void pausar(void) {
    char buffer[16];

    printf("\nPresione Enter para continuar...");
    fflush(stdout);

    (void)leer_linea(buffer, sizeof buffer);
}
