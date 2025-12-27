#ifndef DRV_CONSUMO_H
#define DRV_CONSUMO_H

#include <stdint.h>

// Inicializa el driver de consumo.
// "monitor_id": monitor que se marcará en caso de errores (si se usa).
void drv_consumo_iniciar(uint8_t monitor_id);

// Esperar en bajo consumo (System ON, usando WFI).
void drv_consumo_esperar(void);

// Dormir en consumo mínimo (System OFF).
void drv_consumo_dormir(void);


#endif // DRV_CONSUMO_H

