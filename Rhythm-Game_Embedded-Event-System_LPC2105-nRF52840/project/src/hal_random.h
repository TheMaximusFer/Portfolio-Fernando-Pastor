#ifndef HAL_RANDOM_H
#define HAL_RANDOM_H

#include <stdint.h>

/*
 * Inicializa la fuente de entropía hardware.
 * Debe llamarse una vez al inicio antes de pedir números.
 */
void hal_random_iniciar(void);

/*
 * Devuelve un número de 1 byte aleatorio obtenido del hardware.
 */
uint8_t hal_random_generar_u8(void);

#endif /* HAL_RANDOM_H */

