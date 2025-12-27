/* *****************************************************************************
 * P.H.2025: TODO
 * implementacion para cumplir el hal_tiempo.h
 */
 
#include <nrf.h>
#include "hal_tiempo.h"
#include "drv_monitor.h"
 
 /*typedef struct {
    uint32_t ticks_per_us;   / ticks de HW por microsegundo (p.ej., 15 a 15 MHz) 
    uint8_t  counter_bits;   / ancho del contador libre (p.ej., 32) 
    uint32_t counter_max;    / (1u<<counter_bits)-1 
} hal_tiempo_info_t; */

static volatile uint64_t tiempo_64b = 0;


/*
 * Inicializa el tick y devuelve parámetros de conversión 
 * entre ellas, devuelve la constante ticks_per_us,
 * ticks_per_us permite pasar de los ticks del hardware a microsegundos
 */
void hal_tiempo_iniciar_tick(hal_tiempo_info_t *out_info){
	
	// Parar y limpiar por si ya corría.
  NRF_TIMER0->TASKS_STOP  = 1;
  NRF_TIMER0->TASKS_CLEAR = 1; 
		
	// Configuramos el timer.
	NRF_TIMER0->MODE = 0; 							//Modo timer.
	NRF_TIMER0->BITMODE = 3;						//32 bits de reloj.
	NRF_TIMER0->PRESCALER = 0;					//16MHz => 1 tick =  0.0625 µs
	
	//Creamos sistema de interrupciones para registrar los overflow.
	//Ajustamos el overflow al comparar.
	NRF_TIMER0->CC[0] = 0xFFFFFFFF;			//Cada vez que llegamos al o 2^32-1 saltamos a la IRQ.	
	NRF_TIMER0->EVENTS_COMPARE[0] = 0;
  (void)NRF_TIMER0->EVENTS_COMPARE[0];
	//Short, al comparar en CC0, hacer CLEAR (reinicia a 0).
	NRF_TIMER0->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk;;//Hacemos que el timer se reinicie solo al llegar a 2^32-1.
	//Habilitamos IRQ de COMPARE0.
	NRF_TIMER0->INTENSET = TIMER_INTENSET_COMPARE0_Msk; ;	//Habilatamos las interrupciones para el CC[0].
    NVIC_ClearPendingIRQ(TIMER0_IRQn);//Limpiamos pendientes.
    NVIC_EnableIRQ(TIMER0_IRQn);			//Conectamos la función a la interrupción.


	//Empezamos la ejecución del timer.
	NRF_TIMER0->TASKS_START = 1;	
	
	//Devolvemos información en out_info.
	out_info->ticks_per_us = 16;				//16 tick HW por microsegundo.
	out_info->counter_bits = 32;  			//Ancho de 32 bits de contador.
	out_info->counter_max = 0xFFFFFFFF;
	
}

//Función que se ejecuta para gestionar el overflow del reloj.
void TIMER0_IRQHandler(void){
	if (NRF_TIMER0->EVENTS_COMPARE[0]) {
				drv_monitor_marcar(2);
				NRF_TIMER0->EVENTS_COMPARE[0] = 0;
				(void)NRF_TIMER0->EVENTS_COMPARE[0];
				tiempo_64b += (1ULL << 32);
		drv_monitor_desmarcar(2);
	 }
}

//Lectura del tick de 64 bits.
uint64_t hal_tiempo_actual_tick64(void){

	uint64_t t1, t2;										//Variables temporales para asegurarnos de que no hay overflow entre medias.
	uint32_t ahora;
	do{
			t1 = tiempo_64b;
			NRF_TIMER0->TASKS_CAPTURE[3] = 1; //Tomamos el tiempo en este momento.
			ahora  = NRF_TIMER0->CC[3];				//Guardamos en ahora el valor actual del reloj.
			t2 = tiempo_64b;
	} while (t1 != t2);               	//si hubo overflow, reintenta.
	return t1 + (uint64_t)ahora;				//Devolvemos los ticks actuales.


}




/* --- Reloj periódico por IRQ --- */
static void (*s_cb)(void) = 0;   			//Puntero a la función callback del usuario.


/* --- Interrupción del TIMER1 --- */
void TIMER1_IRQHandler(void) {
	if (NRF_TIMER1->EVENTS_COMPARE[0]) {
			drv_monitor_marcar(1);
			NRF_TIMER1->EVENTS_COMPARE[0] = 0;  //Limpiamos flag.
			(void)NRF_TIMER1->EVENTS_COMPARE[0];

			if (s_cb) s_cb(); 							//Llamamos al callback del usuario.
			drv_monitor_desmarcar(1);
	}
}

/* Configura el periodo en ticks (no arranca) */
void hal_tiempo_periodico_config_tick(uint32_t periodo_en_tick){

	NRF_TIMER1->TASKS_STOP  = 1;
	NRF_TIMER1->TASKS_CLEAR = 1;

	if (periodo_en_tick == 0) return;

	//Configuramos TIMER1.
	NRF_TIMER1->MODE = 0;   						//Modo Timer.
	NRF_TIMER1->BITMODE = 3; 						//32 bits de reloj.
	NRF_TIMER1->PRESCALER = 0;   				//16MHz = 0,0625 s => 16 tick = 1 us

	//Configuramos el "match" en CC[0].
	NRF_TIMER1->CC[0] = periodo_en_tick - 1;   //Interrupción cada periodo_en_tick µs.

	//Cuando el contador == CC[0], genera evento e interrumpe + reinicia.
	NRF_TIMER1->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk; //Auto-reset al llegar a CC[0].
	NRF_TIMER1->INTENSET = TIMER_INTENSET_COMPARE0_Msk;   //Habilitamos interrupción en CC[0].

	//Limpiamos flags anteriores.
	NRF_TIMER1->EVENTS_COMPARE[0] = 0;
	(void)NRF_TIMER1->EVENTS_COMPARE[0];

	//Configuramos NVIC.
	NVIC_ClearPendingIRQ(TIMER1_IRQn);
	NVIC_EnableIRQ(TIMER1_IRQn);
		
}


/* Registra el callback llamado desde la IRQ */
void hal_tiempo_periodico_set_callback(void (*cb)()){
	
	s_cb = cb;
	
}

/* Habilita/Deshabilita el reloj periódico */
void hal_tiempo_periodico_enable(bool enable){

	if (enable) {
			NRF_TIMER1->TASKS_START = 1;  	//Arranca el timer.
	} else {
			NRF_TIMER1->TASKS_STOP = 1;   	//Lo detiene.
	}

}

/* */
void hal_tiempo_reloj_periodico_tick(uint32_t periodo_en_tick, void(*funcion_callback_drv)()){
		s_cb = funcion_callback_drv;  // guardamos callback del driver

    if (periodo_en_tick == 0) {
        NRF_TIMER1->TASKS_STOP = 1;
        return;
    }

    NRF_TIMER1->TASKS_STOP  = 1;
    NRF_TIMER1->TASKS_CLEAR = 1;

    NRF_TIMER1->MODE = 0;
    NRF_TIMER1->BITMODE = 3;
    NRF_TIMER1->PRESCALER = 0;       

    NRF_TIMER1->CC[0] = periodo_en_tick - 1;

    NRF_TIMER1->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk;
    NRF_TIMER1->EVENTS_COMPARE[0] = 0;
    NRF_TIMER1->INTENSET = TIMER_INTENSET_COMPARE0_Msk;

    NVIC_ClearPendingIRQ(TIMER1_IRQn);
    NVIC_EnableIRQ(TIMER1_IRQn);

    NRF_TIMER1->TASKS_START = 1;
}


