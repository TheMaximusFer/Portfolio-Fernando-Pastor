#ifndef DRV_BOTONES_H
#define DRV_BOTONES_H

#include <stdint.h>
#include "rt_evento_t.h"

//Tipo de callback para las tareas suscritas.
typedef void (*drv_botones_callback_t)(EVENTO_T evento, uint32_t auxData);

//Inicialización general del módulo.
void drv_botones_iniciar(drv_botones_callback_t, EVENTO_T ev1, EVENTO_T ev2);

//Llamada desde la RSI del HAL al detectar una pulsación.
void drv_botones_pulsar_RSI(uint8_t id_boton);

//Función que recibe eventos periódicos o de alarma (ev_BOTON_RETARDO, ev_PULSAR_BOTON).
void drv_botones_actualizar(EVENTO_T evento, uint32_t auxiliar);

#endif
