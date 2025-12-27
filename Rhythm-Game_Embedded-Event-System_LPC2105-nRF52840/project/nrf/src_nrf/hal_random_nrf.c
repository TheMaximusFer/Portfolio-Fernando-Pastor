#include "hal_random.h"
#include <nrf.h>

void hal_random_iniciar(void) {
    /*
		 * Configurar el generador de números aleatorios.
     * Activar corrección de sesgo.
     */
		NRF_RNG->CONFIG = RNG_CONFIG_DERCEN_Enabled;
    
    //Arrancar la tarea.
    NRF_RNG->TASKS_START = 1;
}

uint8_t hal_random_generar_u8(void) {
    //Limpiar evento previo por seguridad.
    NRF_RNG->EVENTS_VALRDY = 0;
    
    // Esperar a que haya un valor listo. El tiempo es indeterminado pero rápido para 1 byte.
    while (NRF_RNG->EVENTS_VALRDY == 0);
    
    //Limpiar evento para la próxima lectura
    NRF_RNG->EVENTS_VALRDY = 0;
    
    //Leer el valor del registro VALUE.
    return (uint8_t)(NRF_RNG->VALUE);
}

