#ifndef RT_EVENTO_T_H
#define RT_EVENTO_T_H

#include <stdint.h>
#include "drv_tiempo.h"

//Tipo de evento.
typedef enum {
    ev_VOID = 0,    				//evento nulo.
    ev_T_PERIODICO = 1,    	//temporizador periódico.
		ev_BOTON_PULSAR = 2, 		//botón pulsado.
		ev_INACTIVIDAD = 3,			//inactividad
		ev_BOTON_RETARDO = 4,   //gestión rebotes botón
		ev_JUEGO_TICK = 5,   		//temporización de juegos.
		ev_PULSACION_LARGA = 6  //pulsacion de 3 segundos o más de un botón.
} EVENTO_T;

//Número de tipos de eventos definidos.
#define ev_NUM_EV_USUARIO 1
#define ev_USUARIO {ev_BOTON_PULSAR}
#define EVENT_TYPES 7

#endif // RT_EVENTO_T_H
