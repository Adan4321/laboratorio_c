#include <stdio.h>
#include <math.h>
#include <windows.h>
#include "cabeceras.h"
#include "validaciones.h"

void mostrar_menu(void) {
    printf("\n=================================\n");
    printf("      SIMULADOR ARTEMIS\n");
    printf("=================================\n");
    printf("1- Emular Ángulo Desplazado\n");
    printf("2- Avanzar durante un lapso de tiempo\n");
    printf("3- Cambiar Órbita\n");
    printf("4- Ejercer Pulso Lateral\n");
    printf("5- Panel de Estado\n");
    printf("6- Calcular fuerza centrípeta (demostración)\n");
    printf("7- Ver Reporte\n");
    printf("8- Salir\n");
    printf("---------------------------------\n");
}

int main(void) {
    SetConsoleOutputCP(CP_UTF8);

    maniobra *prim = NULL;
    maniobra *ult = NULL;
    artemis nave;
    nave.radio_ini = R_ORB;
    nave.radio_act = R_ORB;
    nave.alt_ini = R_ORB - R_T;
    nave.alt_act = nave.alt_ini;
    int id_cant = 1;   // proximo id de maniobra
    int opc = 0;

    do {
        limpiar_pantalla();
        mostrar_menu();
        opc = leer_entero("Elija una opción (1-8): ", 1, 8);
        limpiar_pantalla();
        switch (opc) {
            case 1: {
                int sub_opc;
                double t_lapso = 0, angulo = 0, dist_arco = 0;
                printf("--- EMULAR ÁNGULO DESPLAZADO ---\n");
                printf("1- Determinar ángulo desplazado en función del tiempo\n");
                printf("2- Determinar tiempo en función de un ángulo\n");
                sub_opc = leer_entero("Elija qué dato obtener (1-2): ", 1, 2);
                if (sub_opc == 1) {
                    t_lapso = leer_double("Tiempo en segundos (1 a 86400): ", 1, 86400);
                    angulo = Obtener_Angulo_Recorrido(nave.radio_act, t_lapso);
                } else {
                    angulo = leer_double("Ángulo en radianes (0.01 a 62.8): ", 0.01, 62.8);
                    t_lapso = Tiempo_Para_Recorrer(nave.radio_act, angulo);
                }
                dist_arco = Recorrido(nave.radio_act, t_lapso) / 1000.0;
                printf("\n-- ÁNGULO: %.4f rad | Tiempo: %.2f s | Arco: %.2f km\n", angulo, t_lapso, dist_arco);
                pausar_pantalla();
                break;
            }
            case 2: {
                printf("--- AVANZAR DURANTE UN LAPSO DE TIEMPO ---\n");
                double segundos = leer_double("Segundos a avanzar (1 a 86400): ", 1, 86400);
                double dist = Recorrido(nave.radio_act, segundos);
                printf("\nSe avanzaron %.2f s. Vueltas: %.2f. Distancia: %.2f m\n",
                       segundos, Cant_Vueltas(nave.radio_act, segundos), dist);
                maniobra m = {0};
                m.id = id_cant++;
                m.tipo = TIPO_AVANCE;
                m.desplazamiento = dist;
                m.tiempo = segundos;
                m.aceleracion = 0.0;
                m.v_maniobra_max = Velocidad(nave.radio_act);
                m.v_orbital_final = Velocidad(nave.radio_act);
                m.radio_final = nave.radio_act;
                m.altura_final = nave.alt_act;
                agregar_maniobra(&prim, &ult, m);
                pausar_pantalla();
                break;
            }
            case 3: {
                printf("--- CAMBIAR ÓRBITA ---\n");
                printf("Altura actual: %.2f m (permitido: %.0f a %.0f m)\n", nave.alt_act, ALT_MIN, ALT_MAX);
                double delta_R = leer_double("Variación de radio en metros (ej. 50000 o -20000): ", ALT_MIN - nave.alt_act, ALT_MAX - nave.alt_act);
                double t_maniobra = leer_double("Tiempo de la maniobra en segundos (2 a 600): ", TIEMPO_MIN, TIEMPO_MAX);
                double acel = Obtener_Aceleracion(delta_R, t_maniobra);
                double v_max = mostrar_trayectoria(delta_R, t_maniobra);
                nave.radio_act += delta_R;
                nave.alt_act = nave.radio_act - R_T;
                printf("\nMANIOBRA COMPLETADA - Nuevo radio: %.2f m | Nueva altura: %.2f m\n", nave.radio_act, nave.alt_act);
                printf("Aceleración: %.4f m/s^2 | Velocidad máx. de maniobra: %.2f m/s\n", acel, v_max);
                printf("Nueva velocidad orbital (MCU): %.2f m/s\n", Velocidad(nave.radio_act));
                maniobra m = {0};
                m.id = id_cant++;
                m.tipo = TIPO_ORBITA;
                m.desplazamiento = delta_R;
                m.tiempo = t_maniobra;
                m.aceleracion = acel;
                m.v_maniobra_max = v_max;
                m.v_orbital_final = Velocidad(nave.radio_act);
                m.radio_final = nave.radio_act;
                m.altura_final = nave.alt_act;
                agregar_maniobra(&prim, &ult, m);
                pausar_pantalla();
                break;
            }
            case 4: {
                double a, dt, v_lat, desvio, v_res, ang, exc;
                printf("--- PULSO LATERAL ---\n");
                a  = leer_double("Aceleración del pulso en m/s^2 (0.1 a 5): ", 0.1, 5);
                dt = leer_double("Duración del pulso en segundos (1 a 60): ", 1, 60);
                Pulso_Lateral(nave.radio_act, a, dt, &v_lat, &desvio, &v_res, &ang, &exc);
                printf("\nVelocidad orbital antes del pulso: %.2f m/s\n", Velocidad(nave.radio_act));
                printf("Velocidad lateral (a*dt):          %.2f m/s\n", v_lat);
                printf("Desviación lateral (a*dt^2/2):     %.2f m\n", desvio);
                printf("Velocidad resultante:              %.2f m/s\n", v_res);
                printf("Ángulo de desvío:                  %.6f rad (%.4f grados)\n",
                       ang, ang * 180.0 / (DPI / 2));
                printf("Excentricidad resultante:          %.4f\n", exc);
                printf("Velocidad de escape a este radio:  %.2f m/s\n", Velocidad_Escape(nave.radio_act));
                printf("\nTrayectoria: ");
                if (v_res >= Velocidad_Escape(nave.radio_act)) {
                    printf("la nave supera la velocidad de escape y abandona la órbita.\n");
                } else {
                    printf("la órbita deja de ser circular y pasa a ser elíptica.\n");
                }
                maniobra m = {0};
                m.id = id_cant++;
                m.tipo = TIPO_PULSO;
                m.desplazamiento = desvio;
                m.tiempo = dt;
                m.aceleracion = a;
                m.v_maniobra_max = v_lat;
                m.v_orbital_final = v_res;
                m.radio_final = nave.radio_act;
                m.altura_final = nave.alt_act;
                agregar_maniobra(&prim, &ult, m);
                pausar_pantalla();
                break;
            }
            case 5: {
                printf("================================\n");
                printf("      PANEL DE ESTADO ACTUAL\n");
                printf("================================\n");
                printf("Radio Orbital Inicial:  %.2f m\n", nave.radio_ini);
                printf("Radio Orbital Actual:   %.2f m\n", nave.radio_act);
                printf("Altura Inicial:         %.2f m\n", nave.alt_ini);
                printf("Altura Actual:          %.2f m\n", nave.alt_act);
                printf("Velocidad Orbital:      %.2f m/s\n", Velocidad(nave.radio_act));
                printf("Periodo Orbital:        %.2f s (%.2f min)\n", Periodo_Orbital(nave.radio_act), Periodo_Orbital(nave.radio_act) / 60.0);
                printf("Frecuencia:             %.6f Hz\n", Frecuencia(nave.radio_act));
                printf("Velocidad Angular:      %.6f rad/s\n", Velocidad_Angular(nave.radio_act));
                printf("Aceleracion Centripeta: %.4f m/s^2\n", Aceleracion_Centripeta(nave.radio_act));
                printf("Vueltas por Dia:        %.2f\n", Obtener_Vueltas_Diarias(nave.radio_act));
                printf("Velocidad de Escape:    %.2f m/s\n", Velocidad_Escape(nave.radio_act));
                printf("------------------------------------------------\n");
                pausar_pantalla();
                break;
            }
            case 6: {
                double Fg = Fuerza_Gravitatoria(nave.radio_act);
                double Fc = Fuerza_Centripeta(nave.radio_act);
                double dif = fabs(Fg - Fc);
                printf("--- DEMOSTRACIÓN: LA GRAVEDAD ACTÚA COMO FUERZA CENTRÍPETA ---\n");
                printf("Datos: r = %.2f m | m = %.1f kg\n\n", nave.radio_act, M_O);
                printf("1) Fuerza gravitatoria:  Fg = G*MT*m / r^2\n");
                printf("   Fg = %.4f N\n", Fg);
                printf("2) Velocidad orbital que mantiene el MCU:  v = sqrt(G*MT / r)\n");
                printf("   v  = %.4f m/s\n", Velocidad(nave.radio_act));
                printf("3) Fuerza centrípeta necesaria para ese MCU:  Fc = m*v^2 / r\n");
                printf("   Fc = %.4f N\n\n", Fc);
                printf("Diferencia |Fg - Fc| = %.4f N\n", dif);
                if (dif < 0.01) {
                    printf("Entonces Fg = Fc: la atracción gravitatoria es exactamente la fuerza centrípeta.\n");
                } else {
                    printf("Entonces las fuerzas no coinciden: revisar los datos.\n");
                }
                printf("Reemplazando v^2 = G*MT/r en Fc se obtiene Fc = G*MT*m/r^2 = Fg.\n");
                printf("Aceleración centrípeta: v^2/r = %.4f m/s^2\n", Aceleracion_Centripeta(nave.radio_act));
                pausar_pantalla();
                break;
            }
            case 7:
                menu_reporte(&nave, prim, ult, id_cant - 1);
                break;
            case 8:
                if (!leer_confirmacion("¿Está seguro de que desea salir del simulador?")) {
                    opc = 0; // cancela la salida y vuelve al menu
                } else {
                    printf("Gracias por Utilizar el Simulador Artemis.\n");
                }
                break;
        }
    } while (opc != 8);

    liberar_lista(&prim, &ult);   // liberamos la memoria de la lista antes de salir
    return 0;
}
