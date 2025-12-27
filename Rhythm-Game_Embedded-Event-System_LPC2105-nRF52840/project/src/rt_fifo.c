#include "rt_fifo.h"
#include "drv_tiempo.h"
#include "drv_monitor.h"
#include "rt_sc.h" 

//Parámetros de la cola.
#ifndef RT_FIFO_CAPACITY
#define RT_FIFO_CAPACITY 32
#endif


//Estado interno.
typedef struct {
	EVENTO eventos_encolados[RT_FIFO_CAPACITY];
	volatile uint32_t head; 								//próxima posición libre para escribir
	volatile uint32_t tail; 								//próxima posición con dato para leer (main)
	uint32_t estadisticas[EVENT_TYPES]; 		//contadores por tipo
	uint32_t total; 												//total encolados histórico
	uint32_t mon_overflow; 									//monitor a marcar en overflow (0 => ninguno)
	volatile uint32_t contador;							//contador actual de elementos encolados del 1 al RT_FIFO_CAPACITY, 0 si cola vacía
} fifo_t;


static fifo_t cola;


void rt_FIFO_inicializar(uint32_t monitor_overflow) {
    cola.head = 0;
    cola.tail = 0;
	  cola.contador = 0;
    for (uint32_t i = 0; i < EVENT_TYPES; ++i) cola.estadisticas[i] = 0;
    cola.total = 0;
    cola.mon_overflow = monitor_overflow;
}


void rt_FIFO_encolar(uint32_t ID_evento, uint32_t auxData) {
	
    //entrar en sección crítica
		rt_sc_estado_t sc = rt_sc_entrar();
	
		if (cola.contador == RT_FIFO_CAPACITY) {
        if (cola.mon_overflow) drv_monitor_marcar(cola.mon_overflow);
        while(true){ /* overflow -> bucle infinito para diagnóstico */ }
    }

    uint32_t pos = cola.head;
    cola.eventos_encolados[pos].ID_EVENTO = (EVENTO_T)ID_evento;
    cola.eventos_encolados[pos].auxData = auxData;
    cola.eventos_encolados[pos].TS = drv_tiempo_actual_us();


    cola.head = (pos + 1) % RT_FIFO_CAPACITY;
		cola.contador++;

    // Estadísticas
    if (ID_evento < EVENT_TYPES) cola.estadisticas[ID_evento]++;
    cola.total++;
		
		//salir de la sección crítica
    rt_sc_salir(sc);
}


uint8_t rt_FIFO_extraer(EVENTO_T *ID_evento, uint32_t *auxData, Tiempo_us_t *TS) {
    
		//entrar en sección crítica
		rt_sc_estado_t sc = rt_sc_entrar();
	
		uint32_t t = cola.tail;
    if (cola.contador == 0){
			rt_sc_salir(sc);
			return (uint8_t)-1;
		}
		
    if (ID_evento) *ID_evento = cola.eventos_encolados[t].ID_EVENTO;
    if (auxData) *auxData = cola.eventos_encolados[t].auxData;
    if (TS) *TS = cola.eventos_encolados[t].TS;

    cola.tail = (t + 1) % RT_FIFO_CAPACITY;
		cola.contador--;
	
    //devolver cantidad de eventos aún sin procesar tras la extracción.
    uint32_t pendientes = cola.contador;
    if (pendientes > 254) pendientes = 254;
		
		//salir de la sección crítica
		rt_sc_salir(sc);
		
		return (uint8_t)pendientes;
}


uint32_t rt_FIFO_estadisticas(EVENTO_T ID_evento) {
    if (ID_evento == ev_VOID) return cola.total;
    if ((uint32_t)ID_evento < EVENT_TYPES) return cola.estadisticas[(uint32_t)ID_evento];
    return 0;
}

