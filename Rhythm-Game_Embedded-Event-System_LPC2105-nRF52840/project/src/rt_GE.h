#ifndef RT_GE_H
#define RT_GE_H

#ifndef rt_GE_MAX_SUSCRITOS
#define rt_GE_MAX_SUSCRITOS 4
#endif

#include <stdint.h>
#include <stddef.h>
#include "rt_evento_t.h"

//Tipo de callback para las tareas suscritas al GE.
typedef void (*rt_GE_callback_t)(EVENTO_T evento, uint32_t auxData);

//Tabla de suscripciones por tipo de evento.
typedef struct {
    rt_GE_callback_t lista_callbacks[rt_GE_MAX_SUSCRITOS];
    uint8_t num_suscritos;
    uint8_t prioridad[rt_GE_MAX_SUSCRITOS];
} GE_suscripcion_t;

extern uint32_t rt_monitor_overflow;
extern GE_suscripcion_t ge_tabla[EVENT_TYPES];

//Inicializa el gestor de eventos.
void rt_GE_iniciar(uint32_t monitor_overflow);

//Bucle principal del gestor de eventos.
void rt_GE_lanzador(void);

//Callback común para eventos de usuario (botones, inactividad, ...).
void rt_GE_actualizar(EVENTO_T ID_EVENTO, uint32_t auxData);

#endif // RT_GE_H
