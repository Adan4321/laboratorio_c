#ifndef ARTEMIS_H
#define ARTEMIS_H

#define G 6.674e-11
#define M_TIERRA 5.972e24
#define R_TIERRA 6371000.0
#define GM (G * M_TIERRA)
#define PI 3.14159265358979323846
#define DOS_PI (2.0 * PI)

#define ALTITUD_MIN_M 160000.0
#define ALTITUD_MAX_M 2000000.0
#define MASA_MIN_KG 1000.0
#define MASA_MAX_KG 100000.0
#define MAX_RESULTADOS 100
#define MAX_EVENTOS 100

typedef struct {
    unsigned turno;
    double tiempo;
    char descripcion[80];
} Evento;

typedef struct {
    double altitud;
    double masa;
    double velocidad;
    double periodo;
    double acel_centripeta;
    double fuerza_grav;
    double velocidad_angular;
    double tiempo_mision;
    unsigned turnos;
    unsigned pulsos;
    int cargada;
    Evento eventos[MAX_EVENTOS];
    int n_eventos;
} Mision;

typedef struct {
    char nombre[40];
    double valor;
    char unidad[12];
} Resultado;

void inicializar_mision(Mision *m);
void configurar_mision(Mision *m, int *n);
void calcular_orbita(Mision *m);

double radio_orbital(const Mision *m);
double velocidad_orbital(const Mision *m);
double periodo_orbital(const Mision *m);
double frecuencia_orbital(const Mision *m);
double velocidad_angular(const Mision *m);
double aceleracion_centripeta(const Mision *m);
double fuerza_gravitatoria(const Mision *m);
double vueltas_diarias(const Mision *m);
double velocidad_escape(const Mision *m);
double angulo_recorrido(const Mision *m, double segundos);
double distancia_recorrida(const Mision *m, double segundos);
double tiempo_para_angulo(const Mision *m, double radianes);
void simular_pulso_lateral(double aceleracion, double duracion, double *vel_lat, double *desvio);

void accion_velocidad(Mision *m, Resultado *res, int *n);
void accion_periodo(Mision *m, Resultado *res, int *n);
void accion_aceleracion(Mision *m, Resultado *res, int *n);
void accion_pulso(Mision *m, Resultado *res, int *n);
void accion_avanzar_orbita(Mision *m, Resultado *res, int *n);
void accion_reporte(Mision *m, const Resultado *res, int n);
void accion_analisis(const Mision *m);
void accion_reiniciar(Mision *m, Resultado *res, int *n);

void registrar(Resultado *res, int *n, const char *nombre, double valor, const char *unidad);
void registrar_evento(Mision *m, const char *descripcion);
void mostrar_estado(const Mision *m);
void mostrar_reporte(const Resultado *res, int n);
void mostrar_bitacora(const Mision *m);
void mostrar_resumen_desempeno(const Mision *m, int n_resultados);

#endif
