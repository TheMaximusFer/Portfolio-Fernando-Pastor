#include "svc_alarmas.h"
#include "drv_monitor.h"
#include "rt_sc.h"

//Estructura  interna
typedef struct{
    bool activa;
    svc_alarma_flags_t flags;       //Codificación de flags de activación.
    uint32_t restante_ms;           //Cuenta atrás en ms.
    EVENTO_T id_evento;             //Evento a encolar al vencer.
    uint32_t aux;                   //auxData que viajará con el evento.
} alarma_t;

typedef struct{
    alarma_t alarmas[svc_ALARMAS_MAX];                      //Todas las alarmas del sistema.
    MONITOR_id_t monitor_overflow;	                    //Monitor que se marca cuando no quedan alarmas.
    void (*cb_encolar)(uint32_t id_ev, uint32_t aux);       //Función de encolar
    EVENTO_T ev_tick;                                   //Evento de tick que dispara la actualización.
} sistema;

static sistema s;

//Funciones auxiliares (internas).
static inline bool es_periodica(svc_alarma_flags_t f) {
    return (f & SVC_ALARMA_FLAG_PERIODICA) != 0;
}

static inline uint32_t periodo_ms(svc_alarma_flags_t f) {
    return (f & SVC_ALARMA_RETARDO_MASK_MS);
}

static int buscar_por_evento(EVENTO_T ev, uint32_t aux)
{
    for (int i = 0; i < (int)svc_ALARMAS_MAX; ++i) {
        if (!s.alarmas[i].activa) continue;
        if (s.alarmas[i].id_evento != ev) continue;

        // Para rebotes de botones, distinguimos también por aux (id de botón)
        if (ev == ev_BOTON_RETARDO) {
            if (s.alarmas[i].aux == aux) return i;
        } else {
            // para el resto de eventos mantenemos la semántica original
            return i;
        }
    }
    return -1;
}


static int buscar_libre(void) {
    for (int i = 0; i < (int)svc_ALARMAS_MAX; ++i) {
        if (!s.alarmas[i].activa) return i;
    }
    return -1;
}

static void disparar(const alarma_t* a) {
    if (s.cb_encolar) {
        s.cb_encolar((uint32_t)a->id_evento, a->aux);
    }
}

//Operaciones públicas
void svc_alarma_iniciar(uint32_t monitor_overflow, void (*cb_encolar)(uint32_t id_ev, uint32_t aux), EVENTO_T ev_a_notificar){
    
    for (int i = 0; i < (int)svc_ALARMAS_MAX; ++i) {
        s.alarmas[i].activa = false;
				s.alarmas[i].flags = 0;
				s.alarmas[i].restante_ms = 0;
				s.alarmas[i].id_evento = ev_VOID;
				s.alarmas[i].aux = 0;
    }

    s.monitor_overflow = monitor_overflow;
    s.cb_encolar = cb_encolar;
    s.ev_tick = ev_a_notificar;
}

void svc_alarma_activar(svc_alarma_flags_t flags, EVENTO_T id_evento, uint32_t auxData){
    rt_sc_estado_t sc = rt_sc_entrar();
    //Buscamos si la alarma del evento existe y está actuiva
		int id_alarma = buscar_por_evento(id_evento, auxData);



    //Si retardo_ms == 0 => CANCELA la alarma (si existe).
    uint32_t retardo = periodo_ms(flags);
    if (retardo == 0) {
        if (id_alarma >= 0) {
            s.alarmas[id_alarma].activa = false;
        }
        return;
    }

    //Si no vamos a reprogramra, buscamos el siguiente espacio de alarma libre.
    if (id_alarma < 0) id_alarma = buscar_libre();

    //Si no hay libres levantamos el monitor.
    if (id_alarma < 0) {
        if (s.monitor_overflow) drv_monitor_marcar(s.monitor_overflow);
        while(true){ /* Todas las alarmas ocupadas */ }
    }

    //Si repogramamos o creamos la alarma, rellenamos con los nuevos datos. 
    s.alarmas[id_alarma].activa = true;
    s.alarmas[id_alarma].flags = flags;
    s.alarmas[id_alarma].restante_ms = retardo;
    s.alarmas[id_alarma].id_evento = id_evento;
    s.alarmas[id_alarma].aux = auxData;
		rt_sc_salir(sc);
}

void svc_alarma_actualizar(EVENTO_T id_evento, uint32_t auxData){

    if (id_evento != s.ev_tick) return;

    //tomamos auxData como ms transcurridos desde el último tick (si viniera a 0, usamos al menos 1).
    uint32_t transcurrido = (auxData == 0) ? 1 : auxData;
	
		rt_sc_estado_t sc = rt_sc_entrar();
		
    for(int i = 0; i < (int)svc_ALARMAS_MAX; ++i){

        if(!s.alarmas[i].activa) continue;

        if(transcurrido >= s.alarmas[i].restante_ms){
            // VENCE
            disparar(&s.alarmas[i]);
            
            if (es_periodica(s.alarmas[i].flags)) {
                //Reprogramación periódica.
                uint32_t periodo = periodo_ms(s.alarmas[i].flags);
                uint32_t overshoot = transcurrido - s.alarmas[i].restante_ms;

                if (periodo == 0) periodo = 1;
                // Si el overshoot es múltiplo del periodo, disparamos una vez y dejamos periodo entero
                s.alarmas[i].restante_ms = periodo - (overshoot % periodo);
                if (s.alarmas[i].restante_ms == 0) s.alarmas[i].restante_ms = periodo;
            } else {
                //Esporódica: se desactiva.
                s.alarmas[i].activa = false;
            }
        }else{
            // Aún no vence: decrementa
            s.alarmas[i].restante_ms -= transcurrido;
        }
    }
		
		rt_sc_salir(sc);
}

