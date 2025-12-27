/* *****************************************************************************
 * P.H.2025: Driver/Manejador del consumo
 * suministra los servicios de iniciar, esperar y domir... independientemente del hardware
 */

#include "drv_consumo.h"
#include "hal_consumo.h"
#include "board.h"
#include "drv_monitor.h"

static MONITOR_id_t id_monitor;

void drv_consumo_iniciar(uint8_t monitor_id){
    hal_consumo_iniciar();
		id_monitor = monitor_id;
}

void drv_consumo_esperar(void){
		drv_monitor_marcar(id_monitor);
    hal_consumo_esperar();
		drv_monitor_desmarcar(id_monitor);
}

void drv_consumo_dormir(void){
		drv_monitor_marcar(id_monitor);
    hal_consumo_dormir();
}

//otras???
