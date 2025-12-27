#include <LPC210x.H>
#include "hal_WDT.h"
#include "rt_sc.h"

void hal_WDT_iniciar(uint32_t timeout_segundos){
	WDTC = timeout_segundos * (15000000 / 4); // time-out WatchDog.
	WDMOD = 0x03; // Habilito y configuro reinicio.
	hal_WDT_feed();
}

void hal_WDT_feed(void){
	
	rt_sc_estado_t sc = rt_sc_entrar();
	
	WDFEED = 0xAA; // Alimento el WatchDog
	WDFEED = 0x55; 
	
	rt_sc_salir(sc);
}



