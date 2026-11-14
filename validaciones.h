#ifndef VALIDACIONES_H
#define VALIDACIONES_H

void limpiar_pantalla(void);
void pausar_pantalla(void);
void limpiar_linea(void);
int leer_entero(const char *msg, int min, int max);
double leer_double(const char *msg, double min, double max);
int leer_confirmacion(const char *msg);

#endif
