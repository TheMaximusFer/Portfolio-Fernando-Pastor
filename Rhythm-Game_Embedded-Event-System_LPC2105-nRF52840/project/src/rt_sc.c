#include <stdint.h>
#include <nrf.h>        // Para __get_PRIMASK, __set_PRIMASK, __disable_irq, __enable_irq
#include "rt_sc.h"

/*
 * - PRIMASK = 1  => IRQs deshabilitadas
 * - PRIMASK = 0  => IRQs habilitadas
 */

rt_sc_estado_t rt_sc_entrar(void)
{
    // Guardamos el estado previo
    uint32_t primask = __get_PRIMASK();
    // Deshabilitamos interrupciones
    __disable_irq();
    return primask;
}

void rt_sc_salir(rt_sc_estado_t estado_prev)
{
    // Restauramos exactamente el estado previo
    __set_PRIMASK(estado_prev);
}
