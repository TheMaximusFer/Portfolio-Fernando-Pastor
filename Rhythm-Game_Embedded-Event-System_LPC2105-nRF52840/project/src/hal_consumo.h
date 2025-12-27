/* *****************************************************************************
 * P.H.2025: HAL de consumo
 */
 
#ifndef HAL_CONSUMO
#define HAL_CONSUMO

#include <stdint.h>
#include <stdbool.h>



/*
 * Inicializa el HAL de consumo.
 */
void hal_consumo_iniciar(void);


/*
 * Pone el procesador en modo de espera (bajo consumo).
 */
void hal_consumo_esperar(void);


/*
 * Pone el procesador en modo de sueño profundo.
 */
void hal_consumo_dormir(void);


#endif // HAL_CONSUMO
