#include "drv_botones.h"
#include "svc_GE.h"
#include "board.h"
#include "hal_ext_int.h"
#include "svc_alarmas.h"
#include "drv_tiempo.h"

#define RETARDO_REBOTE_MS 50    // Tiempo rebote presión
#define RETARDO_SOLTAR_MS 50    // Tiempo rebote al soltar  
#define RETARDO_ENCUESTA_MS 20  // Tiempo encuesta periódica (revisar si sigue pulsado)
#define PULSACION_LARGA 3000    // 3 segundos como pulsación larga

typedef enum {
    e_esperando,
    e_salida,
    e_rebotes,
    e_muestreo,
} boton_estado_t;

typedef struct {
    boton_estado_t estado;
    uint8_t pin;
	  uint32_t inicio_pulsacion; // Timestamp de cuando se ha pulsado el botón
		bool larga_enviada;
} boton_t;

static boton_t botones[BUTTONS_NUMBER] = {
    {e_salida, BUTTON_1},
    {e_salida, BUTTON_2},
    {e_salida, BUTTON_3}
		#if BUTTON_4
		,
    {e_salida, BUTTON_4}
		#endif
};

//Callback a nivel superior.
static drv_botones_callback_t drv_botones_callback;

void drv_botones_iniciar(drv_botones_callback_t botones_callback, EVENTO_T ev1, EVENTO_T ev2){
    drv_botones_callback = botones_callback;
    hal_ext_int_registrar_callback(drv_botones_pulsar_RSI);
    hal_ext_int_iniciar();

    //Estado inicial: esperando pulsaciones.
    for (uint8_t i = 0; i < BUTTONS_NUMBER; ++i) {
        botones[i].estado = e_esperando;
    }

    //Habilitar SENSE (LOW) en todos los botones.
    for (uint8_t i = 1; i <= BUTTONS_NUMBER; ++i) {
        hal_ext_int_habilitar(i);
    }

    svc_GE_suscribir(ev1, 1, drv_botones_actualizar);
    if (ev2 != ev_VOID) {
        svc_GE_suscribir(ev2, 1, drv_botones_actualizar);
    }
}


//Llamada desde la RSI del HAL al detectar una pulsación.
void drv_botones_pulsar_RSI(uint8_t id_boton){
	
	if (id_boton == 0 || id_boton > BUTTONS_NUMBER) return;
	
	//Solo procesar si estamos esperando (evita crear múltiples alarmas)
	if (botones[id_boton - 1].estado != e_esperando) {
		return;  //Ya estamos procesando este botón, ignorar
	}
	
	// Se pulsa el botón y se guarda su timestamp
	botones[id_boton - 1].inicio_pulsacion = drv_tiempo_actual_ms();
	
	//Reseteamos la "flag" de pulsación larga.
	botones[id_boton - 1].larga_enviada = false;
	
	
	//NOTA: Para LPC con interrupciones por nivel, la ISR ya deshabilita la interrupción.
	//Para NRF con interrupciones por flanco, necesitamos deshabilitarla aquí.
	//Como esta función se llama desde la ISR, la interrupción ya está deshabilitada en LPC.
  hal_ext_int_deshabilitar(id_boton);

	svc_alarma_activar(svc_alarma_codificar(0, RETARDO_REBOTE_MS, 0), ev_BOTON_RETARDO, id_boton);
	
  botones[id_boton - 1].estado = e_rebotes;

}

void drv_botones_actualizar(EVENTO_T evento, uint32_t auxiliar){
		uint32_t ahora; // variable para medir el tiempo pulsado del botón
    uint8_t id_boton = (uint8_t)auxiliar;

    //Botón fuera de rango? return...
    if (id_boton == 0 || id_boton > BUTTONS_NUMBER) {
        return;
    }

    switch (evento) {

        //Evento interno: timeout de las alarmas de rebote/soltar.
        case ev_BOTON_RETARDO:
            switch (botones[id_boton - 1].estado){

                //Acabamos de detectar RSI y estamos en antirrebotes.
                case e_rebotes:
                    if (hal_ext_int_obtener_estado(id_boton)) {
						            //Sigue pulsado después del retardo: pulsación válida.
                        botones[id_boton - 1].estado = e_muestreo;

                        if (drv_botones_callback) {
                            drv_botones_callback(ev_BOTON_PULSAR, id_boton);
                        }

                        //Programar alarma para comprobar periódicamente.
                        svc_alarma_activar(svc_alarma_codificar(0, RETARDO_ENCUESTA_MS, 0), ev_BOTON_RETARDO, id_boton);
                    } else {
												//Era ruido/rebote: volver a esperar y re-habilitar.
                        botones[id_boton - 1].estado = e_esperando;
                        hal_ext_int_habilitar(id_boton);
                    }
                    break;

                //Estamos esperando a que se suelte el botón.
                case e_muestreo:
                    ahora = drv_tiempo_actual_ms();
                    
                    //Comprobamos si se sigue pulsando físicamente.
                    if(hal_ext_int_obtener_estado(id_boton)) {
                        
                        //Calculamos cuánto tiempo lleva pulsado.
                        uint32_t duracion = ahora - botones[id_boton - 1].inicio_pulsacion;

                        //Si lleva más de 3 segundos Y todavía no hemos avisado Y que venga del botón 3 o 4 (auxData >= 3).
                        if (duracion >= PULSACION_LARGA && !botones[id_boton - 1].larga_enviada && id_boton >= 3) {
                            
                            //Enviamos el evento al juego.
                            if (drv_botones_callback) {
                                drv_botones_callback(ev_PULSACION_LARGA, id_boton);
                            }
                            
                            //Marcamos que ya lo enviamos para no spamear.
                            botones[id_boton - 1].larga_enviada = true;
                        }

                        //Seguimos muestreando periódicamente mientras siga pulsado.
                        svc_alarma_activar(
                            svc_alarma_codificar(0, RETARDO_ENCUESTA_MS, 0),
                            ev_BOTON_RETARDO,
                            id_boton
                        );

                    } else {
                        //Si ya no está pulsado: pasar a salida.
                        botones[id_boton - 1].estado = e_salida;

                        //Retardo de rebote al soltars.
                        svc_alarma_activar(svc_alarma_codificar(0, RETARDO_SOLTAR_MS, 0), ev_BOTON_RETARDO, id_boton);
                    }
                    break;

                //Tras soltar, esperamos a que se estabilice en reposo antes de rearmar SENSE.
                case e_salida:
                    if (!hal_ext_int_obtener_estado(id_boton)) {
												//listo para nueva pulsación.
                        botones[id_boton - 1].estado = e_esperando;
                        hal_ext_int_habilitar(id_boton);
                    } else {
                        //Si sigue pulsado, volvemos a muestreo.
                        botones[id_boton - 1].estado = e_muestreo;
                        svc_alarma_activar(svc_alarma_codificar(0, RETARDO_ENCUESTA_MS, 0), ev_BOTON_RETARDO, id_boton);
                    }
                    break;

                case e_esperando:
                default:
                   
                    break;
            }
            break;
						
        case ev_BOTON_PULSAR:
            break;

        default:
            break;
    }
}


