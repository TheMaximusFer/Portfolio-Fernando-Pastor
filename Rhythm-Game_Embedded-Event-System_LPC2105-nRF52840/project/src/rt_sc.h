#ifndef RT_SC_H
#define RT_SC_H

#include <stdint.h>

/*
 * Tipo que guarda el estado previo de las interrupciones.
 */
typedef uint32_t rt_sc_estado_t;

/*
 * Entra en sección crítica:
 *  - Guarda el estado actual de interrupciones.
 *  - Deshabilita las IRQ.
 * Devuelve el estado previo para poder restaurarlo al salir.
 */
rt_sc_estado_t rt_sc_entrar(void);

/*
 * Sale de sección crítica:
 *  - Restaura el estado de interrupciones que se pasó como argumento.
 */
void rt_sc_salir(rt_sc_estado_t estado_prev);

#endif // RT_SC_H
