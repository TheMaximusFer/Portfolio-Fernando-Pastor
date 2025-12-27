#ifndef SVC_ALARMA_H
#define SVC_ALARMA_H

#include <stdint.h>
#include <stdbool.h>
#include "drv_tiempo.h"
#include "rt_evento_t.h"

/* 
 * Servicio de alarmas software (independiente del HW).
 * - Hasta svc_ALARMAS_MAX alarmas activas a la vez.
 * - Cada alarma dispara un EVENTO_T en la FIFO mediante un callback de encolado.
 * - Requiere que exista un evento de "tick" periódico encolado por drv_tiempo.
 */

//Máximo de alarmas simultáneas
#define svc_ALARMAS_MAX 6


/* 
 * Codificación de flags de activación
 *  [31]: 1 => periódica, 0 => esporádica
 *  [30:24]: reservado (7 bits)
 *  [23:0]: retardo/periodo en milisegundos (máx 16777215 ms)
 */
typedef uint32_t svc_alarma_flags_t;

#define SVC_ALARMA_FLAG_PERIODICA (0x80000000)
#define SVC_ALARMA_RETARDO_MASK_MS (0x00FFFFFF)

//Crear la codificación de las flags para la alarma.
static inline svc_alarma_flags_t svc_alarma_codificar(bool periodica, uint32_t retardo_ms, uint8_t siete_flags){
    if (retardo_ms > SVC_ALARMA_RETARDO_MASK_MS) retardo_ms = SVC_ALARMA_RETARDO_MASK_MS; //si metemos más retardo del psoible, le ponemos el máximo posible
    
		//			[31]: 1 => periódica, 0 => esporádica 		  [30:24]: reservado (7 bits)						 [23:0]: retardo/periodo en milisegundos
		return ((periodica ? SVC_ALARMA_FLAG_PERIODICA : 0)|(((uint32_t)siete_flags & 0x7F) << 24)|(retardo_ms & SVC_ALARMA_RETARDO_MASK_MS));
}

/* 
 * Inicializa el servicio y arranca el tick periódico HW que encola "ev_tick".
 *  - "monitor_overflow": id de monitor HW para marcar overflow, 0 si no se usa.
 *  - "cb_encolar": rt_FIFO_encolar.
 *  - "ev_a_notificar": ID de evento que el driver de tiempo encolará.
 *  - "periodo_tick_ms": periodo del tick periódico en milisegundos.
 */
void svc_alarma_iniciar(uint32_t monitor_overflow, void (*cb_encolar)(uint32_t id_ev, uint32_t aux), EVENTO_T ev_a_notificar);

/* 
 * Activar o reprogramar una alarma:
 *  - "flags": use svc_alarma_codificar().
 *  - "id_evento"/"auxData": lo que se encolará cuando venza.
 *  - Si retardo_ms == 0 => CANCELA la alarma (si existe).
 */
void svc_alarma_activar(svc_alarma_flags_t flags, EVENTO_T id_evento, uint32_t auxData);


//A invocar en el despachador de eventosdispatcher (lanzador del gestor de eventos) al recibir el tick: actualiza temporizadores.
void svc_alarma_actualizar(EVENTO_T id_evento, uint32_t auxData);


#endif //SVC_ALARMA_H
