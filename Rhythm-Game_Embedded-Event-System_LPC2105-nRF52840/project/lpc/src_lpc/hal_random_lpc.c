#include "hal_random.h"
#include <LPC210x.H>

/* 
 * Necesitamos acceso a algún contador de tiempo para "simular" entropía
 * Se asume que hal_tiempo ha iniciado el Timer 1.
 * Si no, devolveremos un valor fijo y dependeremos de la semilla manual del main.
 */
 
void hal_random_iniciar(void) {
    //En el LPC2105 no hay hardware específico de generador aleatorio a iniciar.
}

uint8_t hal_random_generar_u8(void) {
    
		//Usa el valor actual del Timer 1 (T1TC) como fuente de "ruido".
    uint32_t entropia_simulada = T1TC; 
    
    //Cogemos el byte bajo.
    return (uint8_t)(entropia_simulada & 0xFF);
}
