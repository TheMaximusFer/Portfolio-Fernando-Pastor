/* *****************************************************************************
 * P.H.2025: Interrupciones externas / botones en nRF52840
 * Implementación del HAL para cumplir hal_ext_int.h
 *
 *  - Configuramos los pines de botón como entrada + pull-up interno.
 *  - Usamos el evento "PORT" del periférico GPIO (DETECT) como fuente IRQ.
 *  - (Des)habilitar un botón = ajustar PIN_CNF[n].SENSE (LOW => activo, DISABLED => inactivo).
 *  - En la ISR limpiamos EVENTS_PORT y el LATCH.
 *****************************************************************************/

#include <nrf.h>
#include <stdint.h>
#include "board.h"
#include "hal_ext_int.h"
#include "drv_monitor.h"
#include "drv_botones.h"
#include <stddef.h>

static const uint32_t s_buttons[BUTTONS_NUMBER] = BUTTONS_LIST; 

static hal_ext_int_callback_t s_callback = NULL; 

void hal_ext_int_registrar_callback(hal_ext_int_callback_t cb)
{
    s_callback = cb;
}

//Helpers para desglosar puerto/pin.
typedef struct { 
			uint8_t port; 
			uint8_t pin; 
} gpio_pp_t;

static inline gpio_pp_t split_pin(uint32_t gpio)
{
    gpio_pp_t r;
    r.port = (uint8_t)(gpio >> 5);
    r.pin  = (uint8_t)(gpio & 0x1F);
    return r;
}




//API ------------------------------------------------------------------

void hal_ext_int_iniciar(void){

	//Configurar cada botón como entrada con pull-up; SENSE deshabilitado-
	for (uint32_t i = 0; i < BUTTONS_NUMBER; ++i) {
			gpio_pp_t pp = split_pin(s_buttons[i]);

			if (pp.port == 0) {
					NRF_P0->PIN_CNF[pp.pin] =
							(GPIO_PIN_CNF_DIR_Input     << GPIO_PIN_CNF_DIR_Pos)   |
							(GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |
							(GPIO_PIN_CNF_PULL_Pullup   << GPIO_PIN_CNF_PULL_Pos)  |
							(GPIO_PIN_CNF_DRIVE_S0S1    << GPIO_PIN_CNF_DRIVE_Pos) |
							(GPIO_PIN_CNF_SENSE_Disabled<< GPIO_PIN_CNF_SENSE_Pos);
			} else { /* port 1 */
					NRF_P1->PIN_CNF[pp.pin] =
							(GPIO_PIN_CNF_DIR_Input     << GPIO_PIN_CNF_DIR_Pos)   |
							(GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |
							(GPIO_PIN_CNF_PULL_Pullup   << GPIO_PIN_CNF_PULL_Pos)  |
							(GPIO_PIN_CNF_DRIVE_S0S1    << GPIO_PIN_CNF_DRIVE_Pos) |
							(GPIO_PIN_CNF_SENSE_Disabled<< GPIO_PIN_CNF_SENSE_Pos);
			}
	}

	//Limpiar eventos previos y habilitar IRQ por PORT.
	NRF_GPIOTE->EVENTS_PORT = 0;
	(void)NRF_GPIOTE->EVENTS_PORT;

	//El registro LATCH guarda qué pines han cumplido el SENSE, límpiar.
	NRF_P0->LATCH = 0xFFFFFFFF;
	NRF_P1->LATCH = 0xFFFFFFFF;

	//Habilitar interrupción del evento PORT.
	NRF_GPIOTE->INTENCLR = GPIOTE_INTENCLR_PORT_Msk;
	NRF_GPIOTE->INTENSET = GPIOTE_INTENSET_PORT_Msk;

	NVIC_ClearPendingIRQ(GPIOTE_IRQn);
	NVIC_EnableIRQ(GPIOTE_IRQn);

}

//Deshabilitar SENSE (ese botón deja de generar IRQ).
void hal_ext_int_deshabilitar(uint8_t id_boton){
	if (id_boton == 0 || id_boton > BUTTONS_NUMBER) return;
	gpio_pp_t pp = split_pin(s_buttons[id_boton - 1]);

	if (pp.port == 0) {
			//Mantenemos pull-up y solo cambiamos SENSE.
			uint32_t cnf = NRF_P0->PIN_CNF[pp.pin];
			cnf &= ~GPIO_PIN_CNF_SENSE_Msk;
			cnf |=  (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos);
			NRF_P0->PIN_CNF[pp.pin] = cnf;
	} else {
			uint32_t cnf = NRF_P1->PIN_CNF[pp.pin];
			cnf &= ~GPIO_PIN_CNF_SENSE_Msk;
			cnf |=  (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos);
			NRF_P1->PIN_CNF[pp.pin] = cnf;
	}
}

//Habilita SENSE=LOW (generará IRQ al pulsar (activo a nivel bajo).
void hal_ext_int_habilitar(uint8_t id_boton){
	if (id_boton == 0 || id_boton > BUTTONS_NUMBER) return;
	gpio_pp_t pp = split_pin(s_buttons[id_boton - 1]);

	if (pp.port == 0) {
			uint32_t cnf = NRF_P0->PIN_CNF[pp.pin];
			cnf &= ~GPIO_PIN_CNF_SENSE_Msk;
			cnf |=  (GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos);
			NRF_P0->PIN_CNF[pp.pin] = cnf;
	} else {
			uint32_t cnf = NRF_P1->PIN_CNF[pp.pin];
			cnf &= ~GPIO_PIN_CNF_SENSE_Msk;
			cnf |=  (GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos);
			NRF_P1->PIN_CNF[pp.pin] = cnf;
	}
}

//Devuelve 1 si el botón (1..N) está pulsado lógicamente, 0 si no.
uint8_t hal_ext_int_obtener_estado(uint8_t id_boton){
	if (id_boton == 0 || id_boton > BUTTONS_NUMBER) return 0;

	gpio_pp_t pp = split_pin(s_buttons[id_boton - 1]);
	uint32_t mask = (1UL << pp.pin);

	uint32_t level;
	if (pp.port == 0) {
			level = (NRF_P0->IN & mask) ? 1 : 0;
	} else {
			level = (NRF_P1->IN & mask) ? 1 : 0;
	}

	//Los botones son activos a nivel bajo (reposo=1, pulsado=0).
	#if BUTTONS_ACTIVE_STATE == 0
			return (level == 0) ? 1 : 0;
	#else
			return (level != 0) ? 1 : 0;
	#endif

}

//IRQ del evento de puerto (DETECT).
void GPIOTE_IRQHandler(void){
    if (NRF_GPIOTE->EVENTS_PORT) {
        //drv_monitor_marcar(1);

        //Limpiar el evento PORT.
        NRF_GPIOTE->EVENTS_PORT = 0;
        (void)NRF_GPIOTE->EVENTS_PORT;

        //Leer y limpiar LATCH de ambos puertos.
        uint32_t lat0 = NRF_P0->LATCH;
        uint32_t lat1 = NRF_P1->LATCH;
        NRF_P0->LATCH = lat0;
        NRF_P1->LATCH = lat1;


        for (uint32_t i = 0; i < BUTTONS_NUMBER; ++i) {
						gpio_pp_t pp = split_pin(s_buttons[i]);
						uint32_t mask = 1 << pp.pin;

						bool disparado = (pp.port == 0) ? (lat0 & mask) : (lat1 & mask);
						if (disparado) {
								uint8_t id = (uint8_t)(i + 1);
                                // Usamos el callback registrado por el driver
								if (s_callback) s_callback(id); 
						}
				}


        //drv_monitor_desmarcar(1);
    }
}
