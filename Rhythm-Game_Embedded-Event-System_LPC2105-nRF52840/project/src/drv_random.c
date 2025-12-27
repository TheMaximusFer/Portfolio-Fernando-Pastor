#include "drv_random.h"
#include "hal_random.h"
#include <stdlib.h> // Para rand() y srand()

void drv_random_iniciar(uint32_t semilla_manual){
    //Inicializamos el hardware subyacente.
    hal_random_iniciar();

    //Intentar obtener entropía del hardware para la semilla.
    uint32_t semilla_hw = 0;
    
    //Obtenemos 4 bytes para formar una palabra de 32 bits.
    for(int i=0; i<4; i++){
        semilla_hw <<= 8;
        semilla_hw |= hal_random_generar_u8();
    }

    //Si el hardware devolvió 0 (caso LPC simulado sin ruido), usamos la manual.
    if (semilla_hw == 0){
        srand(semilla_manual);
    } else {
        srand(semilla_hw);
    }
}

uint32_t drv_random_generar_rango(uint32_t max){
    if (max == 0) return 0;
    
    /*
		 * Usamos rand() de la librería estándar. 
     * Al haber hecho srand() con entropía hardware (en nRF),
     * la secuencia será impredecible en cada reinicio.
		 */
		return (uint32_t)(rand() % max);
}


