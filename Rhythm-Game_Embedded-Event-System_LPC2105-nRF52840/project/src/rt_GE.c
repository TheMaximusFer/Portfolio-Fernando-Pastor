#include "rt_fifo.h"
#include "drv_tiempo.h"
#include "drv_monitor.h"
#include "drv_consumo.h"
#include "rt_GE.h"
#include "svc_GE.h"
#include "rt_evento_t.h"
#include "svc_alarmas.h"
#include "board.h"
#include "hal_ext_int.h"
#include "drv_WDT.h"


// Estructura para almacenar las suscripciones
uint32_t rt_monitor_overflow;
GE_suscripcion_t ge_tabla[EVENT_TYPES];
Tiempo_us_t tiempo_max_sin_tratar;
uint32_t eventos_tratados;
Tiempo_us_t tiempo_sin_tratar;
Tiempo_us_t media_tiempo_sin_tratar;



void rt_GE_iniciar(uint32_t monitor_overflow){
  tiempo_max_sin_tratar = 0; 
	eventos_tratados = 0;
	media_tiempo_sin_tratar = 0;
	tiempo_sin_tratar = 0;
	rt_monitor_overflow = monitor_overflow;
	for (uint8_t i = 0; i < EVENT_TYPES; i++) {
    ge_tabla[i].num_suscritos = 0;
    for (uint8_t j = 0; j < rt_GE_MAX_SUSCRITOS; j++) {
			ge_tabla[i].lista_callbacks[j] = NULL;
    }
  }
	
}


void rt_GE_lanzador(){
	
	svc_alarma_activar(svc_alarma_codificar(0,20*1000,0), ev_INACTIVIDAD, 0); //alarma por inactividad a los 20 segundos sin hacer nada.
	EVENTO_T id_ev;
  uint32_t aux;
  Tiempo_us_t ts;
	while(1){
		
		drv_WDT_alimentar(); //vamos alimentando al watchdog
		
		//si hay eventos en FIFO
		if(rt_FIFO_extraer(&id_ev, &aux, &ts) != (uint8_t)-1){
				//Actualizar estadísticas
				Tiempo_us_t ahora = drv_tiempo_actual_us();
				Tiempo_us_t delta = ahora - ts;
				eventos_tratados++;
				tiempo_sin_tratar += delta;
				media_tiempo_sin_tratar = tiempo_sin_tratar / eventos_tratados;
				if (delta > tiempo_max_sin_tratar) tiempo_max_sin_tratar = delta;
				//recorer fila del evento llamando funciones
				 if ((uint8_t)id_ev < EVENT_TYPES) {
             	for (uint8_t i = 0; i < ge_tabla[(uint8_t)id_ev].num_suscritos; i++) {
								if (ge_tabla[(uint8_t)id_ev].lista_callbacks[i] != NULL) {
									ge_tabla[(uint8_t)id_ev].lista_callbacks[i](id_ev, aux);
								}
							}
				 }
		} else{
				//sino esperar
				drv_consumo_esperar();
		}
	}

}

//helper
static void preparar_despertar_inactividad(void){
#if BUTTONS_NUMBER > 0
    for (uint8_t i = 1; i <= BUTTONS_NUMBER; ++i) {
        hal_ext_int_habilitar(i);   // SENSE=LOW en TODOS los botones
    }
#endif
}


static uint32_t contador_alarma = 0; 

//Reprograma 
void rt_GE_actualizar(EVENTO_T ID_EVENTO, uint32_t auxData){
		
    switch((uint8_t)ID_EVENTO){
        case ev_INACTIVIDAD:
            preparar_despertar_inactividad();
            drv_consumo_dormir();
            break;
        
        case ev_BOTON_PULSAR:
            contador_alarma++;
            svc_alarma_activar(
                svc_alarma_codificar(0, 20000, 0),
                ev_INACTIVIDAD,
                contador_alarma
            );
            break;

        default:
            break;
    }
}


