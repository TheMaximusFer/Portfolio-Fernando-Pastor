#include <LPC210x.H>
#include <stdint.h>
#include "hal_ext_int.h"
#include "hal_gpio.h"
#include "board.h"

/* ------------ Mapeo de botones a GPIO y a líneas EINT ------------*/

#if BUTTONS_NUMBER > 0
static const uint8_t s_buttons[BUTTONS_NUMBER] = BUTTONS_LIST;
#endif

/* pin = EINT */
static uint8_t pin_to_eint(uint8_t pin)
{
    if (pin == INT_EXT1) return 1; // EINT1
    if (pin == INT_EXT2) return 2; // EINT2
    if (pin == INT_EXT3) return 0; // EINT0
    return 0xFF;
}

/* eint = id botón (1..N) */
static uint8_t eint_to_button(uint8_t eint)
{
    for (uint8_t i = 0; i < BUTTONS_NUMBER; i++) {
        if (pin_to_eint(s_buttons[i]) == eint)
            return i + 1;
    }
    return 0;
}

/* ------------  Declaraciones de handlers ------------ */
void EINT0_ISR(void) __irq;
void EINT1_ISR(void) __irq;
void EINT2_ISR(void) __irq;

/* Callback */
static hal_ext_int_callback_t s_callback = 0;

/* ------------ CONFIGURACIÓN DE PINSEL SEGÚN EINT O GPIO ------------ */

static void configurar_pin_como_eint(uint8_t pin)
{
    if (pin == 14) {           // EINT1
        PINSEL0 &= ~(3u << 28);
        PINSEL0 |=  (2u << 28);
    }
    else if (pin == 15) {      // EINT2
        PINSEL0 &= ~(3u << 30);
        PINSEL0 |=  (2u << 30);
    }
    else if (pin == 16) {      // EINT0
        PINSEL1 &= ~(3u << 0);
        PINSEL1 |=  (1u << 0);
    }
}

static void configurar_pin_como_gpio(uint8_t pin)
{
    if (pin == 14) {           // GPIO
        PINSEL0 &= ~(3u << 28);
    }
    else if (pin == 15) {
        PINSEL0 &= ~(3u << 30);
    }
    else if (pin == 16) {
        PINSEL1 &= ~(3u << 0);
    }
}

/* ------------ INICIALIZACIÓN ------------*/

void hal_ext_int_iniciar(void)
{
    /* Todos los pines como GPIO entrada (necesario para Keil) */
    for (uint32_t i = 0; i < BUTTONS_NUMBER; i++)
        configurar_pin_como_gpio(s_buttons[i]);

    /* Limpiar EXTINT */
    EXTINT = (1 << 0) | (1 << 1) | (1 << 2);

    VICVectAddr2 = (unsigned long)EINT0_ISR;
    VICVectCntl2 = 0x20 | 14;

    VICVectAddr3 = (unsigned long)EINT1_ISR;
    VICVectCntl3 = 0x20 | 15;

    VICVectAddr4 = (unsigned long)EINT2_ISR;
    VICVectCntl4 = 0x20 | 16;

}

/* ------------ CALLBACK ------------ */

void hal_ext_int_registrar_callback(hal_ext_int_callback_t cb)
{
    s_callback = cb;
}

/* ------------HABILITAR/DESHABILITAR INTERRUPCIÓN ------------ */

void hal_ext_int_habilitar(uint8_t id_boton)
{
    if (id_boton == 0 || id_boton > BUTTONS_NUMBER) return;

    uint8_t pin  = s_buttons[id_boton - 1];
    uint8_t eint = pin_to_eint(pin);

    configurar_pin_como_eint(pin);

    EXTINT = (1 << eint);

    VICIntEnable |= (1 << (14 + eint));
}

void hal_ext_int_deshabilitar(uint8_t id_boton)
{
    if (id_boton == 0 || id_boton > BUTTONS_NUMBER) return;

    uint8_t pin  = s_buttons[id_boton - 1];
    uint8_t eint = pin_to_eint(pin);

    VICIntEnClr = (1 << (14 + eint));

    configurar_pin_como_gpio(pin);
}

/* ------------ LEER ESTADO DEL BOTÓN (SIEMPRE EN MODO GPIO) ------------*/

uint8_t hal_ext_int_obtener_estado(uint8_t id_boton)
{
    if (id_boton == 0 || id_boton > BUTTONS_NUMBER)
        return 0;

    uint8_t pin = s_buttons[id_boton - 1];

    /* Leer GPIO real */
    uint32_t val = hal_gpio_leer(pin);

#if BUTTONS_ACTIVE_STATE == 0
    return (val == 0);
#else
    return (val != 0);
#endif
}

/* ------------ HANDLERS DE INTERRUPCIÓN ------------*/

void EINT0_ISR(void) __irq
{
    EXTINT = (1 << 0);
    uint8_t id = eint_to_button(0);
    if (s_callback && id) s_callback(id);
    VICVectAddr = 0;
}

void EINT1_ISR(void) __irq
{
    EXTINT = (1 << 1);
    uint8_t id = eint_to_button(1);
    if (s_callback && id) s_callback(id);
    VICVectAddr = 0;
}

void EINT2_ISR(void) __irq
{
    EXTINT = (1 << 2);
    uint8_t id = eint_to_button(2);
    if (s_callback && id) s_callback(id);
    VICVectAddr = 0;
}
