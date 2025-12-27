#include "drv_monitor.h"
#include "svc_GE.h"
#include "svc_alarmas.h"
#include "rt_GE.h"



// Suscribir una tarea a un evento concreto
void svc_GE_suscribir(EVENTO_T id_evento, uint8_t prioridad, rt_GE_callback_t callback){
	if (id_evento >= EVENT_TYPES || callback == NULL) return;

  GE_suscripcion_t *sus = &ge_tabla[id_evento];

    // Si ya está lleno, overflow
  if (sus->num_suscritos >= rt_GE_MAX_SUSCRITOS) {
    drv_monitor_marcar(rt_monitor_overflow);
    while (1); //detener el sistema
  }

  uint8_t pos = 0;

	// Buscar la posición donde insertar según prioridad
  while (pos < sus->num_suscritos && prioridad >= sus->prioridad[pos]) {
    pos++;
	}

  // Desplazar los elementos a la derecha desde pos
	for (int8_t i = sus->num_suscritos - 1; i >= (int8_t)pos; i--) {
		sus->lista_callbacks[i + 1] = sus->lista_callbacks[i];
		sus->prioridad[i + 1] = sus->prioridad[i];
  }

	// Insertar nuevo callback
  sus->lista_callbacks[pos] = callback;
  sus->prioridad[pos] = prioridad;
	sus->num_suscritos++;
}

// Cancelar suscripción
void svc_GE_cancelar(EVENTO_T id_evento, rt_GE_callback_t callback){
	
	if (id_evento >= EVENT_TYPES || callback == NULL)
        return;

  GE_suscripcion_t *sus = &ge_tabla[id_evento];

  for (uint8_t i = 0; i < sus->num_suscritos; i++) {
    if (sus->lista_callbacks[i] == callback){ 
				for (uint8_t j = i; j + 1 < sus->num_suscritos; j++) {
            sus->lista_callbacks[j] = sus->lista_callbacks[j + 1];
            sus->prioridad[j] = sus->prioridad[j + 1];
        }
				sus->num_suscritos--;
				break;
    }
	}
}

