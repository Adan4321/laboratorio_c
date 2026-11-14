#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "cabeceras.h"
#include "validaciones.h"

// Todas las funciones reciben el radio actual (R_ACT) por valor, en double

double Velocidad(double R_ACT){
    return sqrt(G_M_T / R_ACT);
} // velocidad orbital: raiz de GMT sobre el radio de la orbita

double Periodo_Orbital(double R_ACT){
    return (DPI * R_ACT) / Velocidad(R_ACT);
} // cuanto tarda en dar una vuelta completa

double Frecuencia(double R_ACT){
    return 1 / Periodo_Orbital(R_ACT);
} // vueltas por segundo

double Velocidad_Angular(double R_ACT){
    return Velocidad(R_ACT) / R_ACT;
}

double Aceleracion_Centripeta(double R_ACT){
    return G_M_T / (R_ACT * R_ACT);
}

double Fuerza_Gravitatoria(double R_ACT){
    return M_O * Aceleracion_Centripeta(R_ACT);
} // F = G*MT*m / r^2

double Fuerza_Centripeta(double R_ACT){
    double v = Velocidad(R_ACT);
    return M_O * v * v / R_ACT;
} // F = m*v^2 / r, calculada de forma independiente a la gravitatoria

double Obtener_Vueltas_Diarias(double R_ACT){
    return 86400 / Periodo_Orbital(R_ACT);
}

double Velocidad_Escape(double R_ACT){
    return sqrt(2) * Velocidad(R_ACT);
}

double Obtener_Angulo_Recorrido(double R_ACT, double Lapso){
    return Velocidad_Angular(R_ACT) * Lapso;
} // angulo entre posicion inicial y final tras un lapso

double Recorrido(double R_ACT, double Lapso){
    return Obtener_Angulo_Recorrido(R_ACT, Lapso) * R_ACT;
} // longitud del arco recorrido

double Tiempo_Para_Recorrer(double R_ACT, double Angulo_Recorrer){
    return Angulo_Recorrer / Velocidad_Angular(R_ACT);
} // lapso que toma recorrer un angulo

double Cant_Vueltas(double R_ACT, double Lapso){
    return Lapso / Periodo_Orbital(R_ACT);
}

// Maniobras de cambio de orbita
// Se divide el desplazamiento en 2 mitades: acelera durante T/2 y frena durante T/2

double Obtener_Aceleracion(double Cambio_Orbital, double Tiempo){
    return Cambio_Orbital / ((Tiempo * Tiempo) / 4);
} // a = 4d / T^2

// Desplazamiento desde el inicio de la maniobra (t entre 0 y T/2)
double Desplazamiento_Acelerando(double Cambio_Orbital, double Tiempo, double t){
    return (Obtener_Aceleracion(Cambio_Orbital, Tiempo) / 2) * (t * t);
}

double Velocidad_Acelerando(double Cambio_Orbital, double Tiempo, double t){
    return Obtener_Aceleracion(Cambio_Orbital, Tiempo) * t;
}

// Desplazamiento dentro de la segunda mitad (t se mide desde T/2)
double Desplazamiento_Desacelerando(double Cambio_Orbital, double Tiempo, double t){
    return Cambio_Orbital / (Tiempo / 2) * t
           - (Obtener_Aceleracion(Cambio_Orbital, Tiempo) / 2) * (t * t);
}

double Velocidad_Desacelerando(double Cambio_Orbital, double Tiempo, double t){
    return Cambio_Orbital / (Tiempo / 2)
           - Obtener_Aceleracion(Cambio_Orbital, Tiempo) * t;
}

// Simula la maniobra y llena los arreglos pos[] y vel[]
int recalcular(double Cambio_Orbital, double Tiempo, double paso, double pos[], double vel[], int max){
    int k;
    int n = (int)ceil(Tiempo / paso);
    double t, s;
    if (n < 1) {
        n = 1;
    } else if (n > max - 1) {
        n = max - 1; // no escribir fuera del arreglo
    }
    for (k = 0; k <= n; k++){
        t = (k * Tiempo) / n;
        if (t <= (Tiempo / 2.0)){
            pos[k] = Desplazamiento_Acelerando(Cambio_Orbital, Tiempo, t);
            vel[k] = Velocidad_Acelerando(Cambio_Orbital, Tiempo, t);
        } else {
            s = t - (Tiempo / 2.0);
            pos[k] = Cambio_Orbital / 2.0 + Desplazamiento_Desacelerando(Cambio_Orbital, Tiempo, s);
            vel[k] = Velocidad_Desacelerando(Cambio_Orbital, Tiempo, s);
        }
    }
    return n + 1;
}

// Pulso lateral: aceleracion a constante perpendicular a la velocidad orbital
void Pulso_Lateral(double R_ACT, double a, double dt, double *v_lat, double *desvio, double *v_res, double *ang, double *exc){
    double v = Velocidad(R_ACT);
    *v_lat  = a * dt;
    *desvio = 0.5 * a * dt * dt;
    *v_res  = sqrt(v * v + (*v_lat) * (*v_lat));
    *ang    = atan(*v_lat / v);
    *exc    = (*v_lat / v) * (*v_lat / v);
}

// Lista doble de maniobras, trayectoria y reportes

const char *NOMBRES_TIPO[N_TIPOS] = {"Avance", "Cambio de orbita", "Pulso lateral"};

// Lista doblemente enlazada

void agregar_maniobra(maniobra **prim, maniobra **ult, maniobra datos) {
    maniobra *Q = (maniobra *) malloc(sizeof(maniobra));
    if (Q == NULL) {
        printf("Error: Memoria Insuficiente\n");
        return;
    }
    *Q = datos;
    Q->prox = NULL;
    Q->ant = NULL;
    if (*prim == NULL) {
        *prim = Q;
        *ult = Q;
    } else {
        Q->ant = *ult;
        (*ult)->prox = Q;
        *ult = Q;
    }
}

void liberar_lista(maniobra **prim, maniobra **ult) {
    maniobra *act = *prim;
    while (act != NULL) {
        maniobra *sig = act->prox; // guardar antes de liberar
        free(act);
        act = sig;
    }
    *prim = NULL;
    *ult = NULL;
}

// Trayectoria (usa los arreglos pos[] y vel[])

// Simula la maniobra, muestra unas 10 filas y devuelve la velocidad maxima (|v|)
double mostrar_trayectoria(double Cambio_Orbital, double Tiempo) {
    double pos[MAX_PASOS], vel[MAX_PASOS];
    int n = recalcular(Cambio_Orbital, Tiempo, PASO_SIM, pos, vel, MAX_PASOS);
    double v_max = 0.0;
    int k, salto = (n - 1) / 10;
    double t;
    char fase[10];
    if (salto < 1) {
        salto = 1;
    }

    for (k = 0; k < n; k++) {
        if (fabs(vel[k]) > v_max) {
            v_max = fabs(vel[k]);
        }
    }

    printf("\n  %-9s %-6s %-16s %-14s\n", "t (s)", "Fase", "Posicion (m)", "Veloc. (m/s)");
    for (k = 0; k < n; k++) {
        if (k % salto == 0 || k == n - 1) {
            t = (k * Tiempo) / (n - 1);
            if (t <= Tiempo / 2.0) {
                strcpy(fase, "Acel");
            } else {
                strcpy(fase, "Frena");
            }
            printf("  %-9.1f %-6s %-16.2f %-14.2f\n", t, fase, pos[k], vel[k]);
        }
    }
    printf("  Control: posicion final simulada = %.2f m (pedido: %.2f m)\n", pos[n - 1], Cambio_Orbital);
    return v_max;
}

// Reportes

void encabezado_tabla(void) {
    printf("\n%-4s %-17s %-14s %-10s %-12s %-14s %-14s\n", "ID", "Tipo", "Desplaz.(m)", "Tiempo(s)", "Acel(m/s^2)", "V_Orb_Fin(m/s)", "Radio_Fin(m)");
    printf("---------------------------------------------------------------------------------------------\n");
}

void imprimir_fila(maniobra *p) {
    printf("%-4d %-17s %-14.2f %-10.2f %-12.4f %-14.2f %-14.2f\n", p->id, NOMBRES_TIPO[p->tipo], p->desplazamiento, p->tiempo, p->aceleracion, p->v_orbital_final, p->radio_final);
}

void reporte_buscar(maniobra *prim, int total) {
    int id_a_buscar = leer_entero("\nID de la maniobra a buscar: ", 1, total);
    maniobra *act = prim;
    while (act != NULL && act->id != id_a_buscar) {
        act = act->prox;
    }
    if (act == NULL) {
        printf("Maniobra con ID %d no encontrada.\n", id_a_buscar);
        return;
    }
    printf("\n=========================================\n");
    printf("         DETALLE MANIOBRA ID: %d\n", act->id);
    printf("=========================================\n");
    printf("Tipo:                %s\n", NOMBRES_TIPO[act->tipo]);
    printf("Desplazamiento:      %.2f m\n", act->desplazamiento);
    printf("Tiempo invertido:    %.2f s\n", act->tiempo);
    printf("Aceleracion:         %.4f m/s^2\n", act->aceleracion);
    printf("V. Max Maniobra:     %.2f m/s\n", act->v_maniobra_max);
    printf("V. Orbital Final:    %.2f m/s\n", act->v_orbital_final);
    printf("Radio Final:         %.2f m\n", act->radio_final);
    printf("Altura Final:        %.2f m\n", act->altura_final);
    if (act->tipo == TIPO_ORBITA) {
        printf("\nTrayectoria de la maniobra (resimulada):");
        mostrar_trayectoria(act->desplazamiento, act->tiempo);
    }
    printf("-----------------------------------------\n");
}

void reporte_promedios(maniobra *prim, maniobra *ult, int total) {
    int n = leer_entero("\nCuantas ultimas maniobras promediar? (0 = TODAS): ", 0, total);
    maniobra *act;
    int cont = 0;
    double suma_despl = 0.0, suma_tiempo = 0.0;
    // Si pide 0 se promedian todas recorriendo desde el principio con prox
    if (n <= 0) {
        act = prim;
        while (act != NULL) {
            suma_despl += act->desplazamiento;
            suma_tiempo += act->tiempo;
            cont++;
            act = act->prox;
        }
    } else {
        // Si pide N ultimas, recorremos desde el final hacia atras con ant
        act = ult;
        while (act != NULL && cont < n) {
            suma_despl += act->desplazamiento;
            suma_tiempo += act->tiempo;
            cont++;
            act = act->ant;
        }
    }
    if (cont > 0) {
        printf("\n--- Promedios sobre %d maniobra(s) ---\n", cont);
        printf("-> Desplazamiento promedio: %.2f m\n", suma_despl / cont);
        printf("-> Tiempo promedio:         %.2f s\n", suma_tiempo / cont);
    }
}

void reporte_extremos(maniobra *prim, int total) {
    int id_ini = leer_entero("\nID inicial del rango: ", 1, total);
    int id_fin = leer_entero("ID final del rango: ", id_ini, total);
    maniobra *act = prim;
    int cont = 0;
    int primer_nodo = 1;
    double max_v = 0.0, min_v = 0.0, max_acel = 0.0, max_despl = 0.0;
    double max_radio = 0.0, min_radio = 0.0;

    while (act != NULL) {
        if (act->id >= id_ini && act->id <= id_fin) {
            // Inicializamos los maximos y minimos con los datos del primer nodo del rango
            if (primer_nodo) {
                max_v = act->v_orbital_final;
                min_v = act->v_orbital_final;
                max_acel = fabs(act->aceleracion);
                max_despl = fabs(act->desplazamiento);
                max_radio = act->radio_final;
                min_radio = act->radio_final;
                primer_nodo = 0;
            } else {
                if (act->v_orbital_final > max_v) max_v = act->v_orbital_final;
                if (act->v_orbital_final < min_v) min_v = act->v_orbital_final;
                if (fabs(act->aceleracion) > max_acel) max_acel = fabs(act->aceleracion);
                if (fabs(act->desplazamiento) > max_despl) max_despl = fabs(act->desplazamiento);
                if (act->radio_final > max_radio) max_radio = act->radio_final;
                if (act->radio_final < min_radio) min_radio = act->radio_final;
            }
            cont++;
        }
        act = act->prox;
    }

    if (cont == 0) {
        printf("No se encontraron maniobras en el rango [%d - %d].\n", id_ini, id_fin);
        return;
    }
    printf("\n===================================================\n");
    printf("   ESTADISTICAS EN RANGO [ID %d - %d] (%d nodos)\n", id_ini, id_fin, cont);
    printf("===================================================\n");
    printf("Velocidad Orbital Maxima:     %.2f m/s\n", max_v);
    printf("Velocidad Orbital Minima:     %.2f m/s\n", min_v);
    printf("Aceleracion Maxima (modulo):  %.4f m/s^2\n", max_acel);
    printf("Mayor Desplazamiento (modulo):%.2f m\n", max_despl);
    printf("Radio Orbital Maximo:         %.2f m\n", max_radio);
    printf("Radio Orbital Minimo:         %.2f m\n", min_radio);
}

// Arreglos indexados por el tipo de maniobra: un acumulador por tipo
void reporte_por_tipo(maniobra *prim) {
    int cantidad[N_TIPOS] = {0};
    double suma_tiempo[N_TIPOS] = {0};
    double max_v[N_TIPOS] = {0};
    maniobra *act;
    int i, total = 0;

    for (act = prim; act != NULL; act = act->prox) {
        cantidad[act->tipo]++;
        suma_tiempo[act->tipo] += act->tiempo;
        if (fabs(act->v_maniobra_max) > max_v[act->tipo]) {
            max_v[act->tipo] = fabs(act->v_maniobra_max);
        }
        total++;
    }
    printf("\n%-18s %-9s %-16s %-16s\n", "Tipo", "Cantidad", "Tiempo prom.(s)", "V.max maniobra");
    printf("-------------------------------------------------------------\n");
    for (i = 0; i < N_TIPOS; i++) {
        if (cantidad[i] > 0)
            printf("%-18s %-9d %-16.2f %-16.2f\n", NOMBRES_TIPO[i], cantidad[i], suma_tiempo[i] / cantidad[i], max_v[i]);
        else
            printf("%-18s %-9d %-16s %-16s\n", NOMBRES_TIPO[i], 0, "-", "-");
    }
    printf("Total de maniobras realizadas: %d\n", total);
}

void menu_reporte(const artemis *nave, maniobra *prim, maniobra *ult, int total) {
    maniobra *act;
    int sub_rep;

    if (prim == NULL) {
        printf("\nNo se registraron maniobras todavia.\n");
        pausar_pantalla();
        return;
    }

    do {
        limpiar_pantalla();
        printf("=========================================\n");
        printf("            REPORTE DE LA NAVE\n");
        printf("=========================================\n");
        printf("Estado actual: Radio = %.2f m | Altura = %.2f m\n", nave->radio_act, nave->alt_act);
        printf("Velocidad: %.2f m/s | Periodo: %.2f s | Acel. Centripeta: %.4f m/s^2\n", Velocidad(nave->radio_act), Periodo_Orbital(nave->radio_act), Aceleracion_Centripeta(nave->radio_act));
        printf("Vueltas diarias: %.2f\n", Obtener_Vueltas_Diarias(nave->radio_act));

        printf("\n--- Opciones del Reporte ---\n");
        printf("1. Listado completo de maniobras (cronologico)\n");
        printf("2. Listado en orden inverso (mas reciente primero)\n");
        printf("3. Detalle de una maniobra por ID\n");
        printf("4. Promedios (desplazamiento, tiempo)\n");
        printf("5. Maximos / minimos en un rango de IDs\n");
        printf("6. Estadisticas por tipo de maniobra\n");
        printf("7. Volver al menu principal\n");
        sub_rep = leer_entero("Seleccione opcion (1-7): ", 1, 7);
        limpiar_pantalla();
        switch (sub_rep) {
            case 1:
                encabezado_tabla();
                for (act = prim; act != NULL; act = act->prox) imprimir_fila(act);
                pausar_pantalla();
                break;
            case 2:
                encabezado_tabla();
                for (act = ult; act != NULL; act = act->ant) imprimir_fila(act);
                pausar_pantalla();
                break;
            case 3:
                reporte_buscar(prim, total);
                pausar_pantalla();
                break;
            case 4:
                reporte_promedios(prim, ult, total);
                pausar_pantalla();
                break;
            case 5:
                reporte_extremos(prim, total);
                pausar_pantalla();
                break;
            case 6:
                reporte_por_tipo(prim);
                pausar_pantalla();
                break;
        }
    } while (sub_rep != 7);
}
