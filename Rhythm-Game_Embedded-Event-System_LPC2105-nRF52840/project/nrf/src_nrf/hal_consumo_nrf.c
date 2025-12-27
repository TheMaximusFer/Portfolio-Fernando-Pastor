/* *****************************************************************************
 * P.H.2025: TODO
 */
#include <nrf.h> 

#include "hal_consumo.h"
#include "board.h"
#include <stdint.h>


void hal_consumo_iniciar(void){
	
}


void hal_consumo_esperar(void){
	__WFI();
}



void hal_consumo_dormir(void) {
    NRF_POWER->SYSTEMOFF = POWER_SYSTEMOFF_SYSTEMOFF_Enter;
    __WFE(); 
    while (1) { } //no debería volver de aquí.
}


