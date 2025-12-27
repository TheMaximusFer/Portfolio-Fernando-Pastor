#ifndef HAL_EXT_INT
#define HAL_EXT_INT

#include <stdint.h>
#include <stdbool.h>

typedef void (*hal_ext_int_callback_t)(uint8_t id_boton);

// Inicializa el sistema de interrupciones externas.
void hal_ext_int_iniciar(void);

// Registra la función callback para manejar interrupciones.
void hal_ext_int_registrar_callback(hal_ext_int_callback_t cb);

// Deshabilita la interrupción del botón especificado.
void hal_ext_int_deshabilitar(uint8_t id_boton);

// Habilita la interrupción del botón especificado.
void hal_ext_int_habilitar(uint8_t id_boton);

// Habilita la capacidad de despertar del botón.
void hal_ext_int_habilitar_despertar(uint8_t id_boton);

// Deshabilita la capacidad de despertar del botón.
void hal_ext_int_deshabilitar_despertar(uint8_t id_boton);

// Obtiene el estado actual del botón.
uint8_t hal_ext_int_obtener_estado(uint8_t id_boton);


#endif // HAL_EXT_INT
