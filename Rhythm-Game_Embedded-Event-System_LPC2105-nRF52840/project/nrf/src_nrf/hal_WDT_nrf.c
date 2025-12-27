/* *****************************************************************************
 * P.H.2025: TODO
 */
#include <nrf.h> 

#include "hal_consumo.h"
#include "board.h"
#include <stdint.h>
#include "rt_sc.h"


//Valor necesario para alimentar al watchdog.
#define WDT_FEED_VALUE 0x6E524635

// Usaremos el canal RR0
#define WDT_RR_CHANNEL 0

void hal_WDT_feed(void)
{
    rt_sc_estado_t s = rt_sc_entrar();   //guarda PRIMASK y deshabilita IRQ
    NRF_WDT->RR[WDT_RR_CHANNEL] = WDT_FEED_VALUE;
    rt_sc_salir(s);                      //restaura el estado previo
}


void hal_WDT_iniciar(uint32_t timeout_segundos){
	
	// timeout [s] = (CRV + 1) / 32768
  uint32_t crv = (timeout_segundos * 32768) - 1;
	
	// configurar el contador
	NRF_WDT->CRV = crv;
	
	NRF_WDT->RREN = (1 << WDT_RR_CHANNEL);
	
	NRF_WDT->CONFIG = WDT_CONFIG_SLEEP_Msk;
	
	hal_WDT_feed();
	
	NRF_WDT->TASKS_START = 1;
}



