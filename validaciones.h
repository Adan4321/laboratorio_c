#ifndef VALIDACIONES_H
#define VALIDACIONES_H

double leer_double(const char *mensaje, double minimo, double maximo);
int leer_entero(const char *mensaje, int minimo, int maximo);
int leer_confirmacion(const char *mensaje);
void pausar(void);

#endif
