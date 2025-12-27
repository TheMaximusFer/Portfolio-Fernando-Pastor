/* *****************************************************************************
 * P.H.2025: Driver/Manejador de los temporizadores
 * suministra los servicios independientemente del hardware
 *
 * usa los servicos de hal_tiempo.h: 
 */
 
#include "drv_tiempo.h"
#include "hal_tiempo.h"

static	hal_tiempo_info_t s_info;

/**
 * Inicializa el reloj y empieza a contar
 */
bool drv_tiempo_iniciar(void){
	s_info.ticks_per_us = 0;
	hal_tiempo_iniciar_tick(&s_info);
	return (s_info.ticks_per_us!=0);
}

/**
 * tiempo desde que se inicio el temporizador en microsegundos
 */
Tiempo_us_t drv_tiempo_actual_us(void){
	if (s_info.ticks_per_us == 0u) return 0; //Por si acaso llamamos a la función sin iniciar.
	
	uint64_t ticks = hal_tiempo_actual_tick64();
	//Convierto ticks del hardware a microsegundos.
	return (Tiempo_us_t)(ticks / s_info.ticks_per_us);
}

/**
 * tiempo desde que se inicio el temporizador en milisegundos
 */
Tiempo_ms_t drv_tiempo_actual_ms(void){
	if (s_info.ticks_per_us == 0u) return 0; //Por si acaso llamamos a la función sin iniciar.
	
	uint64_t ticks = hal_tiempo_actual_tick64();
	// Convierto ticks del hardware a milisegundos
	return (Tiempo_ms_t)(ticks / (s_info.ticks_per_us*1000ULL));
}

/*
 * retardo: esperar un cierto tiempo en milisegundos
 */
void drv_tiempo_esperar_ms(Tiempo_ms_t ms){
	if (s_info.ticks_per_us == 0u) return ; //Por si acaso llamamos a la función sin iniciar.
	uint64_t espera_ticks = (uint64_t)ms * 1000ULL * (uint64_t)s_info.ticks_per_us;

	uint64_t t_inicial = hal_tiempo_actual_tick64(); //esperamos
	while ((hal_tiempo_actual_tick64() - t_inicial) < espera_ticks){}
}

/*
 * esperar hasta un determinado tiempo (en ms), devuelve el tiempo actual
 */
Tiempo_ms_t drv_tiempo_esperar_hasta_ms(Tiempo_ms_t deadline_ms){
	if (s_info.ticks_per_us == 0u) return 0; //Por si acaso llamamos a la función sin iniciar.
	
	uint64_t t_objetivo = (uint64_t)deadline_ms * 1000ULL * (uint64_t)s_info.ticks_per_us;

	while (hal_tiempo_actual_tick64() < t_objetivo){}//esperamos

	uint64_t t_ahora = hal_tiempo_actual_tick64();
	return (Tiempo_ms_t)(t_ahora / ((uint64_t)s_info.ticks_per_us * 1000ULL));
}

/*
 *  IMPLEMENTACIÓN PARA BLINK_V3
 */


static void (*s_cb_app)(uint32_t,uint32_t) = 0; //callback de la aplicacion
static uint32_t s_id = 0;

static void drv_tiempo_RSI(void) {
   if (s_cb_app)
			s_cb_app(s_id, drv_tiempo_actual_ms());
}

/*
 *
 */
void drv_tiempo_periodico_ms(Tiempo_ms_t ms, void(*funcion_callback_app)(uint32_t,uint32_t), uint32_t ID_evento){
				
	s_cb_app = funcion_callback_app;
	s_id = ID_evento;
				
	uint32_t ticks = ms * 1000 * s_info.ticks_per_us;

  hal_tiempo_reloj_periodico_tick(ticks, drv_tiempo_RSI);
				
}

