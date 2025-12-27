#ifndef DRV_RANDOM_H
#define DRV_RANDOM_H

#include <stdint.h>

/*
 * Inicializa el generador de números aleatorios.
 * Utiliza el HAL para intentar obtener una semilla hardware.
 * Si el HAL no provee entropía, se puede pasar una semilla manual (ej: tiempo).
 */
void drv_random_iniciar(uint32_t semilla_manual);

/*
 * Devuelve un número aleatorio de 4 bytes dentro de un rango específico.
 * Abstrae la lógica de módulo y límites. "max" es el límite superior (inclusivo).
 * Rango [0, max-1].
 */
uint32_t drv_random_generar_rango(uint32_t max);


#endif /* DRV_RANDOM_H */
