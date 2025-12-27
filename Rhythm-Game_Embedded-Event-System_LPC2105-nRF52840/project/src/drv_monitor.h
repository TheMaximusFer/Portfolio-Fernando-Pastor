#ifndef DRV_MONITOR_H
#define DRV_MONITOR_H

#include <stdint.h>

/* Tipo para IDs de MONITOR (1..N) */
typedef uint8_t MONITOR_id_t;

/* Estado lógico del MONITOR */
typedef enum {
    MONITOR_OFF = 0,
    MONITOR_ON  = 1,
} MONITOR_status_t;

//Inicializar monitores, devolvemos el número de monitores disponibles.
uint32_t drv_monitor_iniciar(void);

//Activar monitor (pin a HIGH).
void drv_monitor_marcar(uint32_t id);

//Desactivar monitor (pin a LOW).  
void drv_monitor_desmarcar(uint32_t id);

#endif
