#include "hal_consumo.h"
#include <LPC210x.H>

//Definidas en Startup.s
extern void Reset_Handler(void);

void hal_consumo_iniciar(void) {
}

//Pone al procesador en estado de espera para reducir su consumo.
void hal_consumo_esperar(void)  {
    EXTWAKE = 7;          //EXTINT0,1,2 podrán despertar del idle.
    PCON |= 0x01;         //Idle mode (continúa donde estaba).
}

//Duerme al procesador para minimizar su consumo (power-down + reinicio).
void hal_consumo_dormir(void)  {
    //Configurar qué EINT pueden despertar del power-down.
    EXTWAKE = 7;          //EINT0,1,2 como fuentes de wake-up

    //Entrar en power-down.
    PCON |= 0x02;

    //Al despertar, el código continúa aquí (reinicio).
    Reset_Handler();      //Salto al reset del sistema.
}
