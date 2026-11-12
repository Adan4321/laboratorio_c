#include "cabeceras.h"
#include <math.h>


double Velocidad(){
 return sqrt(G_M_T/r);
} 
double Periodo_Orbital(){
 return (DPI * r)/Velocidad();
}
double Frecuencia(){
 return 1 / Periodo_Orbital() ;
}
double Velocidad_Angular(){
 return Velocidad() / r ;
}
double Aceleracion_Centripeta(){
 return (G_M_T)/(r * r);
}
double Fuerza_Gravitatoria(){
 return M_O * Aceleracion_Centripeta() ;
}
double Vueltas_Diarias(){
 return 86400 / Periodo_Orbital() ;
}
double Velocidad_Escape(){
 return sqrt(2) * Velocidad() ;
}
double Angulo_Recorrido(){
 return Velocidad_Angular() * tiempo ;
}
double Recorrido(){
 return Angulo_Recorrido() * r ;
}
double Tiempo_Para_Recorrer(){
 return 
}
double Cant_Vueltas(){
 return tiempo / Periodo_Orbital() ;
}
double obtener_velocidad(){
 return (cambio_orbital / 2) / 
}
double obtener_aceleración(){
 return velocidad_media 
}