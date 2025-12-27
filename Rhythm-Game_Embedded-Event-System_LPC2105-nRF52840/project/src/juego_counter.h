#ifndef JUEGO_COUNTER_H
#define JUEGO_COUNTER_H

#include <stdint.h>
#include "rt_evento_t.h"

//Inicializa el juego y arranca la primera ronda.
void juego_counter_iniciar(void);

//Callback que se suscribe al GE para procesar eventos del juego.
void juego_counter_actualizar(EVENTO_T evento, uint32_t auxData);

#endif
