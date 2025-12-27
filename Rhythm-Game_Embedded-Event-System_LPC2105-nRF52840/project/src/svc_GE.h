#ifndef SVC_GE_H
#define SVC_GE_H

#include <stdint.h>
#include <stddef.h>
#include "rt_evento_t.h"
#include "rt_GE.h"


//Suscribir una tarea a un evento concreto.
void svc_GE_suscribir(EVENTO_T id_evento, uint8_t prioridad, rt_GE_callback_t callback);

//Cancelar suscripción de una tarea a un evento.
void svc_GE_cancelar(EVENTO_T id_evento, rt_GE_callback_t callback);


#endif // SVC_GE_H

