#include "cabeceras.h"
#include <math.h>


double Velocidad(double *Cambio_Orbital){
 return sqrt(G_M_T / (r + *Cambio_Orbital));
}

double Periodo_Orbital(double *Cambio_Orbital){
 return (DPI * (r + *Cambio_Orbital)) / Velocidad(Cambio_Orbital);
}

double Frecuencia(double *Cambio_Orbital){
 return 1 / Periodo_Orbital(Cambio_Orbital) ;
}

double Velocidad_Angular(double *Cambio_Orbital){
 return Velocidad(Cambio_Orbital) / (r + *Cambio_Orbital) ;
}

double Aceleracion_Centripeta(double *Cambio_Orbital){
 return (G_M_T) / ((r + *Cambio_Orbital) * (r + *Cambio_Orbital));
}

double Fuerza_Gravitatoria(double *Cambio_Orbital){
 return M_O * Aceleracion_Centripeta(Cambio_Orbital) ;
}

double Vueltas_Diarias(double *Cambio_Orbital){
 return 86400 / Periodo_Orbital(Cambio_Orbital) ;
}

double Velocidad_Escape(double *Cambio_Orbital){
 return sqrt(2) * Velocidad(Cambio_Orbital) ;
}


double Angulo_Recorrido(double *Cambio_Orbital, int *Lapso){
 return Velocidad_Angular(Cambio_Orbital) * *Lapso ;
} 

double Recorrido(double *Cambio_Orbital, int *Lapso){
 return Angulo_Recorrido(Cambio_Orbital, Lapso) * (r + *Cambio_Orbital) ;
}

double Tiempo_Para_Recorrer(double *Cambio_Orbital, int *Angulo_Recorrer){
 return *Angulo_Recorrer / Velocidad_Angular(Cambio_Orbital) ;
}

double Cant_Vueltas(double *Cambio_Orbital, int *Lapso){
 return *Lapso / Periodo_Orbital(Cambio_Orbital) ;
}





double Obtener_Aceleracion(double *Cambio_Orbital, double *Tiempo){
 return *Cambio_Orbital / ((*Tiempo * *Tiempo) / 4) ;
} 


double Desplazamiento_Acelerando(double *Cambio_Orbital, double *Tiempo, int i){
 return r + (Obtener_Aceleracion(Cambio_Orbital, Tiempo) / 2 * (i * i)) ;
}

double Velocidad_Acelerando(double *Cambio_Orbital, double *Tiempo, int i){
 return Obtener_Aceleracion(Cambio_Orbital, Tiempo) * i ;
}



 double Desplazamiento_Desacelerando(double *Cambio_Orbital, double *Tiempo, int i){
  return *Cambio_Orbital / (*Tiempo / 2) * i - (Obtener_Aceleracion(Cambio_Orbital, Tiempo) / 2) * (i * i) ;
 }

 double Velocidad_Desacelerando(double *Cambio_Orbital, double *Tiempo, int i){
 return *Cambio_Orbital / (*Tiempo / 2) - Obtener_Aceleracion(Cambio_Orbital, Tiempo) * i ;
 }