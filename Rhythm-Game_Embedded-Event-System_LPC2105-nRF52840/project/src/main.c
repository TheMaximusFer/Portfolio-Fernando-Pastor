/* *****************************************************************************
 * P.H.2025: main.c
 *
 * Programa principal de Proyecto Hardware.
 *
 * Autores: Enrique Torrs, Fernando Pastor Peralta y Guillermo Ledesma Uche.
 * Universidad de Zaragoza
 * ****************************************************************************/

#include <stdint.h>
#include <stdbool.h>

#include "hal_gpio.h"
#include "drv_leds.h"
#include "drv_tiempo.h"
#include "drv_consumo.h"
#include "drv_monitor.h"
#include "drv_botones.h"
#include "rt_fifo.h"
#include "hal_ext_int.h"
#include "board.h"
#include "rt_GE.h"
#include "svc_GE.h"
#include "svc_alarmas.h"
#include "rt_evento_t.h"
#include "drv_WDT.h"
#include "juego_counter.h"
#include "juego_beat.h"
#include "juego_beat.h"
#include "drv_random.h"

#define SESION 8 	// 1 -> blink_v1, 2 -> blink_v2, 3 -> blink_v3, 4 -> blink_v4, 5 -> test cola overflow, 
									// 6 -> blink_v3_bis, 7 -> Bit Counter-Strike, 8 -> Beat Hero

/* Constante de retardo en milisegundos */
#define RETARDO_MS 500u 		 /* ejemplo: medio segundo */

#if SESION == 1
/* *****************************************************************************
 * BLINK v1: parpadeo de un LED conmutando on/off.
 * Retardo por bucle de instrucciones (busy-wait). Solo usa el driver de LEDs.
 * para realizar la primera sesión de la practica
 */
static void blink_v1(LED_id_t id) {

	drv_led_establecer(id, LED_ON);
    while (1) {
        volatile uint32_t tmo = 10000000u;  /* ajustad para vuestro reloj */
        while (tmo--) { /* nop */ }
        (void)drv_led_conmutar(id);
    }
}

#elif SESION == 2
/* *****************************************************************************
 * BLINK v2, parpadeo de un led conmutando on/off 
 * activacion por tiempo, usa tanto manejador del led como el del tiempo
 * para realizar en la segunda sesion de la practica, version a entregar
 */
static void blink_v2(LED_id_t id) {
    Tiempo_ms_t siguiente_activacion;	
		
    /* Encender LED inicial */
    drv_led_establecer(id, LED_ON);
	
		/* Encender Timer inicial */
		drv_tiempo_iniciar();
	
    /* Tomar tiempo de referencia */
    siguiente_activacion = drv_tiempo_actual_ms();
	
    /* Bucle principal: conmutar con temporizador */
    while (true) {
        siguiente_activacion += RETARDO_MS;     /* calcular la siguiente activación */
        drv_tiempo_esperar_hasta_ms(siguiente_activacion);
        (void)drv_led_conmutar(id);

        /* Aquí podrían añadirse otras tareas periódicas */
    }
}


#elif SESION == 3
/* *****************************************************************************
 * BLINK v3, parpadeo de un led conmutando on/off 
 * durmiendo el procesador con drv_consumo
 */
static void blink_v3(LED_id_t id) {
		drv_consumo_iniciar(3);     // Inicializamos módulo consumo (WFI)
	
		drv_led_establecer(id, LED_ON);
	
    drv_tiempo_iniciar();      // Inicializamos sistema de tiempo

       // Encendemos LED inicial

    // Activación periódica de 500 ms ? ejecuta leds_c(3)
    drv_tiempo_periodico_ms(RETARDO_MS, leds_c, id);

    while (true) {
				drv_consumo_esperar();
		}
}

#elif SESION == 4


/* *****************************************************************************
 * BLINK v4, parpadeo de un led conmutando on/off 
 * durmiendo el procesador con drv_consumo y con cola de eventos
 */
static void blink_v4(LED_id_t id_led) {
  
    drv_tiempo_iniciar();
    drv_consumo_iniciar(3); //monitor 3: CPU despierta/durmiente.
    rt_FIFO_inicializar(4); //monitor 4: overflow de la FIFO.
    drv_led_establecer(id_led, LED_ON);

    //Programamos activación periódica: cada RETARDO_MS encola ev_T_PERIODICO.
    drv_tiempo_periodico_ms(RETARDO_MS,rt_FIFO_encolar, ev_T_PERIODICO);

    //Planificador: extrae y atiende eventos, si no hay, dormir CPU
    while (1) {
        EVENTO_T id_ev;
        uint32_t aux;
        Tiempo_us_t ts;

        //rt_FIFO_extraer() devuelve 0 si la cola está vacía.
        while (rt_FIFO_extraer(&id_ev, &aux, &ts) != (uint8_t)-1) {
            if (id_ev == ev_T_PERIODICO) {
                drv_led_conmutar(id_led);
            }
        }
        drv_consumo_esperar();
    }
}


#elif SESION == 5


/* *****************************************************************************
 * Test para comprobar el desbordamiento de la cola de eventos
 */
static void test_cola_overflow() {
  
		rt_FIFO_inicializar(4);

    //Encolamos 32 elementos antes de provocar overflow.
    __disable_irq();
    for (int i = 0; i < 32; ++i) rt_FIFO_encolar(ev_VOID, 0);
    __enable_irq();

    int fin = 1;                // debe alcanzarse (32 OK, no overflow).

    //overflow de verdad
    rt_FIFO_encolar(ev_VOID, fin);
		
		fin = -1;										//nunca debe llegar aquí
}

#elif SESION == 6

/* *****************************************************************************
 * BLINK v3_bis
 * - 10 palpitaciones esperando entre parpadeos en bajo consumo.
 * - Apaga todos los LEDs y entra en espera de dormir.
 * - Despierta por botón y el programa se reinicia.
 */


static volatile uint8_t s_cuenta = 0;
static volatile uint8_t s_fin = 0;  //para dormir

/* Callback periódico: conmutar y contar */
static void blink_v3_bis_cb(uint32_t id_led_u32, uint32_t t_ms_actual)
{
    drv_led_conmutar((LED_id_t)id_led_u32);

    if (++s_cuenta >= 20u) {
        s_fin = 1; //señal al bucle principal
    }
}

static void apagar_todos_los_leds(void)
{
#if LEDS_NUMBER > 0
    for (uint8_t i = 1; i <= LEDS_NUMBER; ++i) {
        drv_led_establecer((LED_id_t)i, LED_OFF);
    }
#endif
}

static void preparar_despertar_por_boton(void){
    hal_ext_int_iniciar();
#if BUTTONS_NUMBER > 0
    /* Habilita SENSE en todos los botones (activo a nivel bajo en la DK) */
    for (uint8_t i = 1; i <= BUTTONS_NUMBER; ++i) {
        hal_ext_int_habilitar(i);
    }
#endif
}

static void blink_v3_bis(LED_id_t id_led){
    //Inicializaciones
    drv_tiempo_iniciar();
    drv_consumo_iniciar(3);
    drv_led_establecer(id_led, LED_ON);

    /* Programa el parpadeo periódico */
    drv_tiempo_periodico_ms((Tiempo_ms_t)RETARDO_MS, blink_v3_bis_cb, (uint32_t)id_led);

    //Bucle principal: System ON (WFI) hasta completar 10 parpadeos (encender y apagar).
    while (!s_fin) {
        drv_consumo_esperar();
    }

    /* Fin de demostración: apaga LEDs y duerme profundamente */
    apagar_todos_los_leds();
    preparar_despertar_por_boton();      /* SENSE listo para wake-up */
    drv_consumo_dormir();                /* System OFF -> reinicio al despertar */

    while (1) { /* no debería continuar aquí */ }
}

#elif SESION == 7

		//Traduce callback de botones (EVENTO_T, aux) a un encolado en la FIFO de eventos.
		static void publicar_evento_desde_boton(EVENTO_T ev, uint32_t aux){
				//Se guarda el tipo de evento y el id de botón en la FIFO:
				rt_FIFO_encolar((uint32_t)ev, aux);
		}
				
		static void tick_alarma_cb(uint32_t id_ev_u32, uint32_t t_ms_actual){
				(void)t_ms_actual; //No lo usamos
				svc_alarma_actualizar((EVENTO_T)id_ev_u32, 1);
		}
		
		static void bit_Counter_Strike(void){
			//Inicialización de la infraestructura.
			drv_tiempo_iniciar();
			drv_leds_iniciar();
			drv_consumo_iniciar(3);

			drv_WDT_iniciar(10);	//Inicia el watchdog para que salte en 10 segundos si nos quedamos colgados.
			
			rt_FIFO_inicializar(0);
			rt_GE_iniciar(0);

			svc_alarma_iniciar(rt_monitor_overflow, rt_FIFO_encolar, ev_T_PERIODICO);

			//Timer periódico que alimenta las alarmas.
			drv_tiempo_periodico_ms(1, tick_alarma_cb, (uint32_t)ev_T_PERIODICO);

			//Suscripciones al GE.
			svc_GE_suscribir(ev_T_PERIODICO,  0, svc_alarma_actualizar);
			svc_GE_suscribir(ev_INACTIVIDAD,  0, rt_GE_actualizar);
			svc_GE_suscribir(ev_BOTON_PULSAR, 1, rt_GE_actualizar);

			//Driver de botones: usa la FIFO como “publicador” de eventos.
			drv_botones_iniciar(publicar_evento_desde_boton, ev_BOTON_RETARDO, ev_VOID);

			//Suscripción del driver de botones al evento de rebote.
			svc_GE_suscribir(ev_BOTON_RETARDO, 1, drv_botones_actualizar);

			//Suscripciones específicas del juego.
			svc_GE_suscribir(ev_BOTON_PULSAR, 2, juego_counter_actualizar);
			svc_GE_suscribir(ev_JUEGO_TICK,   2, juego_counter_actualizar);

			//Arrancar juego Bit Counter-Strike.
			juego_counter_iniciar();

			// Bucle principal del gestor de eventos
			rt_GE_lanzador();
		}

#elif SESION == 8		

		// Traduce callback de botones (EVENTO_T, aux) a un encolado en la FIFO de eventos.
		static void publicar_evento_desde_boton(EVENTO_T ev, uint32_t aux){
				// Se guarda el tipo de evento y el id de botón en la FIFO:
				rt_FIFO_encolar((uint32_t)ev, aux);
		}

		static void tick_alarma_cb(uint32_t id_ev_u32, uint32_t t_ms_actual){
				(void)t_ms_actual; // No lo usamos
				svc_alarma_actualizar((EVENTO_T)id_ev_u32, 1);
		}

		static void beat_Hero(void){
				//Inicialización de la infraestructura.
				drv_tiempo_iniciar();
				drv_leds_iniciar();
				drv_consumo_iniciar(3);

				//Inicializamos el generador de aleatorios.
				//drv_random_iniciar(12345); //Pasamos una semilla arbitraria por si estamos en LPC.

				drv_WDT_iniciar(10); //Inicia el watchdog para que salte en 10 segundos si nos quedamos colgados.
				
				rt_FIFO_inicializar(0);
				rt_GE_iniciar(0);

				svc_alarma_iniciar(rt_monitor_overflow, rt_FIFO_encolar, ev_T_PERIODICO);

				//Timer periódico que alimenta las alarmas.
				drv_tiempo_periodico_ms(1, tick_alarma_cb, (uint32_t)ev_T_PERIODICO);

				//Suscripciones al GE.
				svc_GE_suscribir(ev_T_PERIODICO, 0, svc_alarma_actualizar);
				svc_GE_suscribir(ev_INACTIVIDAD, 0, rt_GE_actualizar);
				svc_GE_suscribir(ev_BOTON_PULSAR, 1, rt_GE_actualizar);

				//Driver de botones: usa la FIFO como “publicador” de eventos.
				drv_botones_iniciar(publicar_evento_desde_boton, ev_BOTON_RETARDO, ev_PULSACION_LARGA);

				//Suscripción del driver de botones al evento de rebote.
				svc_GE_suscribir(ev_BOTON_RETARDO, 1, drv_botones_actualizar);

				//Suscripciones específicas del juego.
				svc_GE_suscribir(ev_BOTON_PULSAR, 2, juego_beat_actualizar);
				svc_GE_suscribir(ev_JUEGO_TICK, 2, juego_beat_actualizar);
				svc_GE_suscribir(ev_PULSACION_LARGA, 2, juego_beat_actualizar); // Para reiniciar partida
				

				//Arrancar juego Beat Hero.
				juego_beat_iniciar();

				// Bucle principal del gestor de eventos
				rt_GE_lanzador();
		}
#endif






/* *****************************************************************************
 * MAIN, Programa principal.
 * para la primera sesion se debe usar la funcion de blink_v1 sin temporizadores
 * para la entrega final se debe incocar blink_v2
 */
int main(void){
	uint32_t Num_Leds;

	hal_gpio_iniciar();	// llamamos a iniciar gpio antesde que lo hagan los drivers
	
	drv_monitor_iniciar(); // llamamos al inicio de todos los monitores
	
	/* Configurar LEDs (IDs válidos: 1..num_leds) */
	Num_Leds = drv_leds_iniciar();
	
	if (Num_Leds > 0){

			#if SESION == 1
				/* Sesión 1: parpadeo por lazo ocupado */
				blink_v1((LED_id_t)1);
			#elif SESION == 2
				/* Sesión 2: temporizador  */
			 blink_v2((LED_id_t)2);
			#elif SESION == 3
				blink_v3((LED_id_t)3);
			#elif SESION == 4
				blink_v4((LED_id_t)4);
			#elif SESION == 5
				test_cola_overflow();
			#elif SESION == 6
				blink_v3_bis((LED_id_t)3);
			#elif SESION == 7
				bit_Counter_Strike();
			#elif SESION == 8
				beat_Hero();
			#endif

			 
    }

    /* En sistemas empotrados normalmente no se retorna */
    while (1) { /* idle */ }
    /* return 0; */
}
