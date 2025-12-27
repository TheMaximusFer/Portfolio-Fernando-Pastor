#include <LPC210x.H>
#include <stdint.h>
#include "rt_sc.h"


rt_sc_estado_t rt_sc_entrar(void){
	__disable_irq();

	//Para esta práctica no usamos el valor previo, asi que devolvemos 0.
	return 0;
}

void rt_sc_salir(rt_sc_estado_t estado_prev){
	(void)estado_prev;   //No lo usamos en esta versión simple.
	__enable_irq();
}
