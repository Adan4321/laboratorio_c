#ifndef CABECERAS_H
#define CABECERAS_H

// Constantes fisicas y del escenario
#define G_M_T  3.9857128e14  // G (6.674e-11) * masa de la Tierra (5.972e24)
#define R_T    6371000.0     // radio de la Tierra en metros
#define R_ORB  6563000.0     // radio orbital inicial = R_T + altura
#define M_O    33000.0       // masa de la nave Orion en kg
#define DPI    6.2831853     // 2 * PI
#define H_INI  192000.0      // altura inicial en metros

// Limites para validar entradas
#define ALT_MIN     160000.0   // altura minima permitida
#define ALT_MAX    2000000.0   // altura maxima permitida
#define TIEMPO_MIN       2.0   // tiempo minimo de maniobra en seg
#define TIEMPO_MAX     600.0   // tiempo maximo de maniobra en seg

// Arreglos de simulacion
#define MAX_PASOS   1001       // tamano de los arreglos pos[] y vel[]
#define PASO_SIM       1.0     // paso de simulacion en segundos

// Tipos de maniobra para identificar cada nodo y arreglos
enum { TIPO_AVANCE = 0, TIPO_ORBITA = 1, TIPO_PULSO = 2, N_TIPOS = 3 };
extern const char *NOMBRES_TIPO[N_TIPOS];   // nombres para mostrar en pantalla

// Estructuras principales
typedef struct maniobra {
    int id;
    int tipo;                 // TIPO_AVANCE, TIPO_ORBITA o TIPO_PULSO
    double desplazamiento;
    double tiempo;
    double aceleracion;
    double v_maniobra_max;
    double v_orbital_final;
    double radio_final;
    double altura_final;
    struct maniobra *prox;
    struct maniobra *ant;
} maniobra;

typedef struct {
    double radio_ini;
    double radio_act;
    double alt_ini;
    double alt_act;
} artemis;

// Funciones de fisica orbital
double Velocidad(double R_ACT);
double Periodo_Orbital(double R_ACT);
double Frecuencia(double R_ACT);
double Velocidad_Angular(double R_ACT);
double Aceleracion_Centripeta(double R_ACT);
double Fuerza_Gravitatoria(double R_ACT);
double Fuerza_Centripeta(double R_ACT);
double Obtener_Vueltas_Diarias(double R_ACT);
double Velocidad_Escape(double R_ACT);
double Obtener_Angulo_Recorrido(double R_ACT, double Lapso);
double Recorrido(double R_ACT, double Lapso);
double Tiempo_Para_Recorrer(double R_ACT, double Angulo_Recorrer);
double Cant_Vueltas(double R_ACT, double Lapso);

// Maniobra de cambio de orbita
double Obtener_Aceleracion(double Cambio_Orbital, double Tiempo);
double Desplazamiento_Acelerando(double Cambio_Orbital, double Tiempo, double t);
double Velocidad_Acelerando(double Cambio_Orbital, double Tiempo, double t);
double Desplazamiento_Desacelerando(double Cambio_Orbital, double Tiempo, double t);
double Velocidad_Desacelerando(double Cambio_Orbital, double Tiempo, double t);
int recalcular(double Cambio_Orbital, double Tiempo, double paso,
               double pos[], double vel[], int max);

// Pulso lateral con retorno por punteros
void Pulso_Lateral(double R_ACT, double a, double dt, double *v_lat,
                   double *desvio, double *v_res, double *ang, double *exc);

// Manejo de la lista doble y reportes
void agregar_maniobra(maniobra **prim, maniobra **ult, maniobra datos);
void liberar_lista(maniobra **prim, maniobra **ult);
double mostrar_trayectoria(double Cambio_Orbital, double Tiempo);
void menu_reporte(const artemis *nave, maniobra *prim, maniobra *ult, int total);

#endif