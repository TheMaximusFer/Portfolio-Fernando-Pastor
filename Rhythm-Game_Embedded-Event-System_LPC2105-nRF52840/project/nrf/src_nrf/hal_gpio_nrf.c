/* *****************************************************************************
 * P.H.2025: TODO
 */
#include <nrf.h> 

#include "hal_gpio.h"

#include <stdint.h>

//Struct para representar el puerto y pin de un determinado gpio
typedef struct { uint8_t port; uint8_t pin; } gpio_desglosado;

/*
/ Función auxiliar:
/ Dado un gpio devuelve un struct en el que se representa puerto y pin.
/ 
/ Idea sacada de board_nrf52840dk.h: 
/ 	@brief Macro for mapping port and pin numbers to values understandable for nrf_gpio functions. 
/ 	#define NRF_GPIO_PIN_MAP(port, pin) (((port) << 5) | ((pin) & 0x1F))
*/
static inline gpio_desglosado desglosar_gpio(HAL_GPIO_PIN_T gpio){
    gpio_desglosado r;
    r.port = (uint8_t)(gpio >> 5);
    r.pin  = (uint8_t)(gpio & 0x1F);
    return r;
}

/*
 * Permite emplear el GPIO y debe ser invocada antes
 * de poder llamar al resto de funciones de la biblioteca.
 * re-configura todos los pines como de entrada (para evitar cortocircuitos)
 */
void hal_gpio_iniciar(void){
  // Reiniciamos los pines todos como entrada (igual al reset) y ponemos todos los out a 0:
	NRF_P0->DIRCLR = 0xFFFFFFFF; // GPIO Port DirectionClear control register.
  NRF_P1->DIRCLR = 0xFFFFFFFF; // Poner pines marcados en modo entrada
	NRF_P0->OUTCLR = 0xFFFFFFFF; // GPIO Port OutClear control register.
  NRF_P1->OUTCLR = 0xFFFFFFFF; // Poner pines marcados a 0
}

/* *****************************************************************************
 * Acceso a los GPIOs 
 */

/**
 * El gpio se configura como entrada o salida según la dirección.
 */
void hal_gpio_sentido(HAL_GPIO_PIN_T gpio, hal_gpio_pin_dir_t direccion){
	
	gpio_desglosado r = desglosar_gpio(gpio);
	uint32_t masc = (1UL << r.pin);
	
	if(r.port == 0){
			if (direccion == HAL_GPIO_PIN_DIR_INPUT){ //Ponemos pin gpio en entrada
					NRF_P0->DIRCLR = masc;
			}
			else if (direccion == HAL_GPIO_PIN_DIR_OUTPUT){ //Ponemos pin gpio en salida
					NRF_P0->DIRSET = masc;
			}	
	}else if(r.port == 1){
			if (direccion == HAL_GPIO_PIN_DIR_INPUT){ //Ponemos pin gpio en entrada
					NRF_P1->DIRCLR = masc;
			}
			else if (direccion == HAL_GPIO_PIN_DIR_OUTPUT){ //Ponemos pin gpio en salida
					NRF_P1->DIRSET = masc;
			}	
	}
}
 


/*
 * La función devuelve un entero (bool) con el valor de los bits indicados.
 */
uint32_t hal_gpio_leer(HAL_GPIO_PIN_T gpio){
	
	gpio_desglosado r = desglosar_gpio(gpio);
	uint32_t masc = (1UL << r.pin);
	
	if(r.port == 0){
		 /* si el pin es salida, lee OUT; si no, IN */
			if (NRF_P0->DIR & masc) {
					return ((NRF_P0->OUT & masc) != 0);
			} else {
					return ((NRF_P0->IN  & masc) != 0);
			}
	}else if(r.port == 1){
		 /* si el pin es salida, lee OUT; si no, IN */
			if (NRF_P1->DIR & masc) {
					return ((NRF_P1->OUT & masc) != 0);
			} else {
					return ((NRF_P1->IN  & masc) != 0);
			}
	}
	return 0;
}


/*
 * Escribe en el gpio el valor
 */
void hal_gpio_escribir(HAL_GPIO_PIN_T gpio, uint32_t valor){
	
	gpio_desglosado r = desglosar_gpio(gpio);
	uint32_t masc = (1UL << r.pin);
	
	if(r.port == 0){
			if ((valor & 0x01) == 0) NRF_P0->OUTCLR = masc;
			else NRF_P0->OUTSET = masc;		
	}else if(r.port == 1){
		 	if ((valor & 0x01) == 0) NRF_P1->OUTCLR = masc;
			else NRF_P1->OUTSET = masc;
	}
}

