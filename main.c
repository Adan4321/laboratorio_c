#include <locale.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "artemis.h"
#include "validaciones.h"

static void mostrar_menu(void) {
    printf("\n================= MENU PRINCIPAL =================\n");
    printf("1) Configurar mision (altitud y masa)\n");
    printf("2) Calcular velocidad orbital\n");
    printf("3) Calcular periodo orbital\n");
    printf("4) Calcular aceleracion centripeta y fuerza gravitatoria\n");
    printf("5) Simular pulso lateral\n");
    printf("6) Avanzar orbita (turno de simulacion)\n");
    printf("7) Ver reporte de la mision\n");
    printf("8) Ver analisis teorico\n");
    printf("9) Reiniciar simulacion\n");
    printf("0) Salir\n");
    printf("==================================================\n");
}

int main(void) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    setlocale(LC_ALL, "");

    Mision mision;
    Resultado resultados[MAX_RESULTADOS];
    int n_resultados = 0;

    inicializar_mision(&mision);

    printf("==================================================\n");
    printf("   SIMULADOR ARTEMIS - ORBITA TERRESTRE BAJA LEO\n");
    printf("==================================================\n");
    printf("Mision inicial: h = %.1f km | m = %.0f kg\n",
           mision.altitud / 1000.0, mision.masa);

    for (;;) {
        mostrar_estado(&mision);
        mostrar_menu();

        int opcion = leer_entero("Seleccione una opcion (0-9): ", 0, 9);

        switch (opcion) {
            case 1:
                configurar_mision(&mision, &n_resultados);
                break;
            case 2:
                accion_velocidad(&mision, resultados, &n_resultados);
                break;
            case 3:
                accion_periodo(&mision, resultados, &n_resultados);
                break;
            case 4:
                accion_aceleracion(&mision, resultados, &n_resultados);
                break;
            case 5:
                accion_pulso(&mision, resultados, &n_resultados);
                break;
            case 6:
                accion_avanzar_orbita(&mision, resultados, &n_resultados);
                break;
            case 7:
                accion_reporte(&mision, resultados, n_resultados);
                break;
            case 8:
                accion_analisis(&mision);
                break;
            case 9:
                accion_reiniciar(&mision, resultados, &n_resultados);
                break;
            case 0:
                if (leer_confirmacion("Desea salir del simulador?")) {
                    printf("Fin de la simulacion. Buen viaje, Orion!\n");
                    return 0;
                }
                printf("Continuando la simulacion.\n");
                break;
        }

        pausar();
    }

    return 0;
}
