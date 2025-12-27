#ifndef RT_FIFO_H
#define RT_FIFO_H


#include <stdint.h>
#include <stddef.h>
#include "rt_evento_t.h"

//Estructura de un evento en la FIFO.
typedef struct {
	EVENTO_T ID_EVENTO;				//tipo de evento.
	uint32_t auxData;					//dato auxiliar.
	Tiempo_us_t TS;						//tiempo en microsegundos.
} EVENTO;

//Inicializa la cola de eventos en estado vacío.
void rt_FIFO_inicializar(uint32_t monitor_overflow);

//Encola un evento en la cola.
//La función añade internamente el sello de tiempo (TS) en microsegundos.
//En caso de cola llena, se marca el monitor configurado y se entra en bucle infinito.
void rt_FIFO_encolar(uint32_t ID_evento, uint32_t auxData);

//Extrae el evento más antiguo de la cola.
//Si la cola está vacía: devuelve -1. Si hay evento: escribe en los parámetros y devuelve el número
//de eventos que siguen pendientes tras esta extracción.
uint8_t rt_FIFO_extraer(EVENTO_T *ID_evento, uint32_t *auxData, Tiempo_us_t *TS);

// Devuelve estadísticas de encolado por tipo de evento.
uint32_t rt_FIFO_estadisticas(EVENTO_T ID_evento);


#endif // RT_FIFO_H

