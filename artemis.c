#include "artemis.h"
#include "validaciones.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static void copiar_texto(char *destino, size_t tam, const char *origen) {
    if (destino == NULL || origen == NULL || tam == 0) {
        return;
    }
    snprintf(destino, tam, "%s", origen);
}

void inicializar_mision(Mision *m) {
    if (m == NULL) {
        return;
    }

    m->altitud = 192000.0;
    m->masa = 33000.0;
    m->tiempo_mision = 0.0;
    m->turnos = 0;
    m->pulsos = 0;
    m->n_eventos = 0;

    calcular_orbita(m);
    registrar_evento(m, "Mision LEO inicializada por defecto (h=192 km, m=33000 kg)");
}

void configurar_mision(Mision *m, int *n) {
    if (m == NULL) {
        return;
    }

    printf("\n--- CONFIGURACION DE LA MISION LEO ---\n");

    double altitud_km = leer_double("Altitud orbital en km (160 a 2000): ",
                                    ALTITUD_MIN_M / 1000.0, ALTITUD_MAX_M / 1000.0);
    double masa = leer_double("Masa de Orion en kg (1000 a 100000): ",
                              MASA_MIN_KG, MASA_MAX_KG);

    if (n != NULL && *n > 0) {
        if (leer_confirmacion("Desea descartar los resultados acumulados?")) {
            *n = 0;
        }
    }

    m->altitud = altitud_km * 1000.0;
    m->masa = masa;
    m->tiempo_mision = 0.0;
    m->turnos = 0;
    m->pulsos = 0;
    m->n_eventos = 0;

    calcular_orbita(m);

    printf("Mision configurada: h = %.1f km | m = %.0f kg\n",
           m->altitud / 1000.0, m->masa);

    char texto[80];
    snprintf(texto, sizeof texto, "Mision configurada: h=%.1f km, m=%.0f kg",
             m->altitud / 1000.0, m->masa);
    registrar_evento(m, texto);
}

void calcular_orbita(Mision *m) {
    if (m == NULL) {
        return;
    }

    m->velocidad = velocidad_orbital(m);
    m->periodo = periodo_orbital(m);
    m->acel_centripeta = aceleracion_centripeta(m);
    m->fuerza_grav = fuerza_gravitatoria(m);
    m->velocidad_angular = velocidad_angular(m);
    m->cargada = 1;
}

double radio_orbital(const Mision *m) {
    return R_TIERRA + m->altitud;
}

double velocidad_orbital(const Mision *m) {
    return sqrt(GM / radio_orbital(m));
}

double periodo_orbital(const Mision *m) {
    return (DOS_PI * radio_orbital(m)) / velocidad_orbital(m);
}

double frecuencia_orbital(const Mision *m) {
    return 1.0 / periodo_orbital(m);
}

double velocidad_angular(const Mision *m) {
    return velocidad_orbital(m) / radio_orbital(m);
}

double aceleracion_centripeta(const Mision *m) {
    double r = radio_orbital(m);
    return GM / (r * r);
}

double fuerza_gravitatoria(const Mision *m) {
    return m->masa * aceleracion_centripeta(m);
}

double vueltas_diarias(const Mision *m) {
    return 86400.0 / periodo_orbital(m);
}

double velocidad_escape(const Mision *m) {
    return sqrt(2.0) * velocidad_orbital(m);
}

double angulo_recorrido(const Mision *m, double segundos) {
    return velocidad_angular(m) * segundos;
}

double distancia_recorrida(const Mision *m, double segundos) {
    return angulo_recorrido(m, segundos) * radio_orbital(m);
}

double tiempo_para_angulo(const Mision *m, double radianes) {
    return radianes / velocidad_angular(m);
}

void simular_pulso_lateral(double aceleracion, double duracion, double *vel_lat, double *desvio) {
    if (vel_lat != NULL) {
        *vel_lat = aceleracion * duracion;
    }
    if (desvio != NULL) {
        *desvio = 0.5 * aceleracion * duracion * duracion;
    }
}

void registrar(Resultado *res, int *n, const char *nombre, double valor, const char *unidad) {
    if (res == NULL || n == NULL || nombre == NULL || unidad == NULL) {
        return;
    }

    if (*n < 0 || *n >= MAX_RESULTADOS) {
        printf("\nReporte lleno: no se pudo registrar '%s'.\n", nombre);
        return;
    }

    copiar_texto(res[*n].nombre, sizeof res[*n].nombre, nombre);
    res[*n].valor = valor;
    copiar_texto(res[*n].unidad, sizeof res[*n].unidad, unidad);
    (*n)++;
}

void registrar_evento(Mision *m, const char *descripcion) {
    if (m == NULL || descripcion == NULL) {
        return;
    }

    if (m->n_eventos < 0 || m->n_eventos >= MAX_EVENTOS) {
        return;
    }

    Evento *e = &m->eventos[m->n_eventos];
    e->turno = m->turnos;
    e->tiempo = m->tiempo_mision;
    copiar_texto(e->descripcion, sizeof e->descripcion, descripcion);
    m->n_eventos++;
}

void accion_velocidad(Mision *m, Resultado *res, int *n) {
    if (m == NULL) {
        return;
    }

    double r = radio_orbital(m);
    double v = velocidad_orbital(m);

    printf("\n--- VELOCIDAD ORBITAL ---\n");
    printf("Radio orbital r = R_T + h = %.1f km\n", r / 1000.0);
    printf("v = sqrt(G*M_T / r) = %.3f m/s (%.4f km/s)\n", v, v / 1000.0);

    registrar(res, n, "Velocidad orbital", v, "m/s");
    registrar_evento(m, "Calculo de velocidad orbital");
}

void accion_periodo(Mision *m, Resultado *res, int *n) {
    if (m == NULL) {
        return;
    }

    double T = periodo_orbital(m);

    printf("\n--- PERIODO ORBITAL ---\n");
    printf("T = 2*pi*r / v = %.3f s\n", T);
    printf("T = %.3f min (%.3f h)\n", T / 60.0, T / 3600.0);

    registrar(res, n, "Periodo orbital", T, "s");
    registrar_evento(m, "Calculo del periodo orbital");
}

void accion_aceleracion(Mision *m, Resultado *res, int *n) {
    if (m == NULL) {
        return;
    }

    double ac = aceleracion_centripeta(m);
    double f = fuerza_gravitatoria(m);

    printf("\n--- ACELERACION CENTRIPETA Y FUERZA GRAVITATORIA ---\n");
    printf("a_c = v^2/r = G*M_T/r^2 = %.6f m/s^2 (%.4f g)\n", ac, ac / 9.80);
    printf("F_g = m*a_c = %.3f N\n", f);

    registrar(res, n, "Aceleracion centripeta", ac, "m/s^2");
    registrar(res, n, "Fuerza gravitatoria", f, "N");
    registrar_evento(m, "Calculo de aceleracion centripeta y fuerza gravitatoria");
}

void accion_pulso(Mision *m, Resultado *res, int *n) {
    if (m == NULL) {
        return;
    }

    printf("\n--- PULSO LATERAL ---\n");

    double a = leer_double("Aceleracion lateral en m/s^2 (0.1 a 5.0): ", 0.1, 5.0);
    double dt = leer_double("Duracion del pulso en s (1 a 60): ", 1.0, 60.0);
    double v_lat;
    double desvio;

    simular_pulso_lateral(a, dt, &v_lat, &desvio);

    printf("v_lat = a*dt = %.3f m/s\n", v_lat);
    printf("Desvio lateral = 0.5*a*dt^2 = %.3f m\n", desvio);
    printf("Trayectoria: el pulso agrega una componente perpendicular a la velocidad orbital;\n");
    printf("la orbita deja de ser circular y la nave se desplaza lateralmente %.3f m.\n", desvio);

    registrar(res, n, "Velocidad lateral", v_lat, "m/s");
    registrar(res, n, "Desvio lateral", desvio, "m");

    m->pulsos++;

    char texto[80];
    snprintf(texto, sizeof texto, "Pulso lateral: a=%.2f m/s2, dt=%.1f s, desvio=%.2f m",
             a, dt, desvio);
    registrar_evento(m, texto);
}

void accion_avanzar_orbita(Mision *m, Resultado *res, int *n) {
    if (m == NULL) {
        return;
    }

    printf("\n--- AVANCE DE ORBITA (TURNO) ---\n");

    int vueltas = leer_entero("Cantidad de orbitas a simular (1 a 1000): ", 1, 1000);
    double tiempo = vueltas * periodo_orbital(m);
    double distancia = distancia_recorrida(m, tiempo);

    m->tiempo_mision += tiempo;
    m->turnos += (unsigned)vueltas;

    printf("Se completaron %d orbita(s).\n", vueltas);
    printf("Tiempo transcurrido: %.3f s (%.3f min)\n", tiempo, tiempo / 60.0);
    printf("Distancia recorrida: %.3f km\n", distancia / 1000.0);
    printf("Tiempo total de mision: %.3f s | Orbitas totales: %u\n",
           m->tiempo_mision, m->turnos);

    registrar(res, n, "Distancia recorrida", distancia, "m");
    registrar(res, n, "Tiempo de mision", m->tiempo_mision, "s");

    char texto[80];
    snprintf(texto, sizeof texto, "Avance de %d orbita(s); tiempo total=%.1f s",
             vueltas, m->tiempo_mision);
    registrar_evento(m, texto);
}

void accion_reporte(Mision *m, const Resultado *res, int n) {
    printf("\n--- REPORTE DE LA MISION ---\n");
    mostrar_estado(m);
    mostrar_reporte(res, n);
    mostrar_resumen_desempeno(m, n);
    mostrar_bitacora(m);
}

void accion_analisis(const Mision *m) {
    if (m == NULL) {
        return;
    }

    printf("\n--- ANALISIS TEORICO (MCU Y GRAVITACION) ---\n");
    printf("1) La fuerza gravitatoria actua como fuerza centripeta:\n");
    printf("   G*M_T*m/r^2 = m*v^2/r  =>  v = sqrt(G*M_T/r)\n");
    printf("   Con h = %.1f km se obtiene v = %.3f m/s (%.4f km/s).\n",
           m->altitud / 1000.0, m->velocidad, m->velocidad / 1000.0);
    printf("2) Periodo orbital: T = 2*pi*r/v = %.3f s = %.3f min.\n",
           m->periodo, m->periodo / 60.0);
    printf("3) Aceleracion centripeta: a_c = v^2/r = %.6f m/s^2.\n",
           m->acel_centripeta);
    printf("4) Pulso lateral de referencia (a = 0.50 m/s^2, dt = 10.0 s):\n");
    printf("   v_lat = 5.000 m/s; desvio = 25.000 m.\n");
    printf("   El movimiento resultante combina la velocidad orbital tangencial con la\n");
    printf("   componente lateral, produciendo una trayectoria curva desplazada.\n");
    printf("Datos adicionales:\n");
    printf("   Frecuencia orbital = %.9f Hz\n", frecuencia_orbital(m));
    printf("   Velocidad angular = %.9f rad/s\n", velocidad_angular(m));
    printf("   Vueltas por dia = %.3f\n", vueltas_diarias(m));
    printf("   Velocidad de escape = %.3f m/s\n", velocidad_escape(m));
    printf("   Fuerza gravitatoria = %.3f N\n", fuerza_gravitatoria(m));
    printf("   Tiempo para recorrer 90 grados = %.3f s\n", tiempo_para_angulo(m, PI / 2.0));
}

void accion_reiniciar(Mision *m, Resultado *res, int *n) {
    (void)res;

    if (m == NULL) {
        return;
    }

    if (!leer_confirmacion("Reiniciar la simulacion y borrar el reporte?")) {
        printf("Operacion cancelada.\n");
        return;
    }

    inicializar_mision(m);

    if (n != NULL) {
        *n = 0;
    }

    printf("Simulacion reiniciada con los valores por defecto (h = 192 km, m = 33000 kg).\n");
}

void mostrar_estado(const Mision *m) {
    if (m == NULL) {
        return;
    }

    printf("\n--- ESTADO DE LA MISION (LEO) ---\n");
    printf("Altitud: %.1f km | Masa: %.0f kg\n", m->altitud / 1000.0, m->masa);
    printf("Velocidad orbital: %.3f m/s\n", m->velocidad);
    printf("Periodo orbital: %.3f s (%.3f min)\n", m->periodo, m->periodo / 60.0);
    printf("Aceleracion centripeta: %.6f m/s^2\n", m->acel_centripeta);
    printf("Fuerza gravitatoria: %.3f N\n", m->fuerza_grav);
    printf("Orbitas completadas: %u | Tiempo de mision: %.2f s\n",
           m->turnos, m->tiempo_mision);
}

void mostrar_reporte(const Resultado *res, int n) {
    if (res == NULL || n <= 0) {
        printf("\nAun no hay resultados registrados.\n");
        return;
    }

    printf("\n=========== REPORTE DE CALCULOS ===========\n");
    printf("%-4s %-30s %18s %-8s\n", "#", "Calculo", "Valor", "Unidad");
    printf("------------------------------------------------------------\n");

    for (int i = 0; i < n; i++) {
        printf("%-4d %-30s %18.4f %-8s\n", i + 1, res[i].nombre, res[i].valor, res[i].unidad);
    }

    printf("------------------------------------------------------------\n");
    printf("Total de calculos: %d\n", n);
}

void mostrar_resumen_desempeno(const Mision *m, int n_resultados) {
    if (m == NULL) {
        return;
    }

    double circunferencia = DOS_PI * radio_orbital(m);
    double distancia = m->turnos * circunferencia;

    printf("\n--- RESUMEN DE DESEMPENO ---\n");
    printf("Altitud final: %.1f km\n", m->altitud / 1000.0);
    printf("Masa de Orion: %.0f kg\n", m->masa);
    printf("Orbitas completadas: %u\n", m->turnos);
    printf("Tiempo de mision: %.2f s (%.2f min)\n", m->tiempo_mision, m->tiempo_mision / 60.0);
    printf("Distancia orbital acumulada: %.2f km\n", distancia / 1000.0);
    printf("Pulsos laterales aplicados: %u\n", m->pulsos);
    printf("Calculos registrados: %d\n", n_resultados);
    printf("Eventos en bitacora: %d\n", m->n_eventos);
}

void mostrar_bitacora(const Mision *m) {
    if (m == NULL || m->n_eventos <= 0) {
        printf("\nBitacora vacia.\n");
        return;
    }

    printf("\n--- BITACORA DE EVENTOS ---\n");
    printf("%-6s %-12s %s\n", "Turno", "Tiempo(s)", "Evento");

    for (int i = 0; i < m->n_eventos; i++) {
        printf("%-6u %-12.2f %s\n",
               m->eventos[i].turno, m->eventos[i].tiempo, m->eventos[i].descripcion);
    }
}
