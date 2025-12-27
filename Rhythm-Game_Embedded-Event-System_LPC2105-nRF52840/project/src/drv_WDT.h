#ifndef DRV_WDT_H
#define DRV_WDT_H

#include <stdint.h>


// Inicializa el Watchdog Timer con un tiempo de espera especificado en segundos.
void drv_WDT_iniciar(uint32_t timeout_s);

// Alimenta el Watchdog Timer para evitar el reinicio del sistema.
void drv_WDT_alimentar(void);


#endif // DRV_WDT_H

