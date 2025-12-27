/* *****************************************************************************
 * P.H.2025: HAL de consumo
 */
 
#ifndef HAL_WDT_H
#define HAL_WDT_H


#include <stdint.h>

/*
 * Inicializa el watchdog hardware con el timeout indicado.
 */
void hal_WDT_iniciar(uint32_t timeout_segundos);

/*
 * Alimenta el watchdog hardware.
 */
void hal_WDT_feed(void);


#endif // HAL_WDT_H
