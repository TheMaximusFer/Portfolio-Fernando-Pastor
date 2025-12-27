#include "juego_counter.h"
#include "drv_leds.h"
#include "svc_alarmas.h"
#include "board.h"
#include <stdint.h>

// ------------------- Tiempos del juego. -------------------

//Tiempo inicial para contestar a cada LED en ms.
#define JUEGO_TIEMPO_BASE_MS 5000
//Tiempo mínimo para contestar (no bajamos de aquí).
#define JUEGO_TIEMPO_MIN_MS 500
//Paso con el que vamos reduciendo el tiempo de respuesta.
#define JUEGO_TIEMPO_STEP_MS 500

//Pausa entre un LED y el siguiente (cuando se apaga).
#define JUEGO_DELAY_ENTRE_BASE_MS 2000
#define JUEGO_DELAY_ENTRE_MIN_MS 80
#define JUEGO_DELAY_ENTRE_STEP_MS 20



// ------------------- Estado interno -------------------------
typedef enum {
    JG_EST_APAGADO = 0,             //Juego apagado.
    JG_EST_LED_ENCENDIDO = 1,       //Esperando que el usuario pulse.
    JG_EST_ESPERA_ENTRE_LEDS = 2,   //Todos apagados, esperando para encender otro.
} juego_estado_t;

typedef struct {
    juego_estado_t estado;          //Estado actual del juego.
    uint8_t num_pares;              //Nº de pares LED-botón válidos.
    uint8_t led_actual;             //Id de led encendido 1..num_pares.
    uint32_t tiempo_respuesta_ms;   //Tiempo para pulsar.
    uint32_t delay_entre_ms;        //Pausa entre LEDs.
}juego_state_t;

static juego_state_t s;             //Estado global del juego.




// --------------- Funciones internas del juego -------------------

static uint8_t juego_siguiente_led(uint8_t led){
    if (s.num_pares == 0) return 0;
    //Ids lógicos 1..num_pares (LED_1 con BUTTON_1, etc.)
    return (uint8_t)((led % s.num_pares) + 1);
}

static void juego_apagar_todos(void){
    for (uint8_t i = 1; i <= LEDS_NUMBER; ++i) {
        drv_led_establecer((LED_id_t)i, LED_OFF);
    }
}

static void juego_encender_led(uint8_t led_id){
    juego_apagar_todos();
    drv_led_establecer((LED_id_t)led_id, LED_ON);
}

static void juego_reprogramar(uint32_t retardo_ms){
    //Programamos una alarma esporádica que encolará un ev_JUEGO_TICK a los "retardo_ms" ms (no usamos monitor).
    svc_alarma_activar(svc_alarma_codificar(0, retardo_ms, 0), ev_JUEGO_TICK, 0);
}




// ------------------- Interfaz pública -----------------------

void juego_counter_iniciar(void){
    #if (LEDS_NUMBER > 0) && (BUTTONS_NUMBER > 0)
        s.num_pares = (LEDS_NUMBER < BUTTONS_NUMBER) ? LEDS_NUMBER : BUTTONS_NUMBER;
    #else
				s.num_pares = 0;
        s.estado = JG_EST_APAGADO;
        juego_apagar_todos();
        return;
    #endif

    s.tiempo_respuesta_ms = JUEGO_TIEMPO_BASE_MS;
    s.delay_entre_ms = JUEGO_DELAY_ENTRE_BASE_MS;

    // Primer LED
    s.led_actual = juego_siguiente_led(0);
    juego_encender_led(s.led_actual);
    s.estado = JG_EST_LED_ENCENDIDO;

    // Programar tiempo de respuesta para este LED
    juego_reprogramar(s.tiempo_respuesta_ms);
}


void juego_counter_actualizar(EVENTO_T evento, uint32_t auxData){
		uint8_t boton = (uint8_t)auxData;
	
		switch ((uint8_t)evento){
        case ev_BOTON_PULSAR: //El jugador ha pulsado un botón.
            

            //Solo tratamos la pulsación del botón si estamos en el estado JG_EST_LED_ENCENDIDO 
            // (esperando a que el usuario pulse el botón).
            if (s.estado != JG_EST_LED_ENCENDIDO) {
                return;
            }

            //Si el usuario pulsa un botón que no esté entre 1..num_pares, no lo tratamos.
            if (boton == 0 || boton > s.num_pares) {
                return;   // pulsación que no usamos
            }

            //Si el botón está entre los pulsables pero no es el iluminado, no lo tratamos, 
            // encendemos y apagamos inmediatamente para indicar error. El estado del juego continua igual.
            if (boton != s.led_actual) {
                LED_id_t led_id = ((LED_id_t)boton);
                drv_led_establecer(led_id, LED_ON);
                volatile uint32_t tmo = 1000000; //Espera activa, para que se note el parpadeo (sin necesidad de tocar relojes ya ajustados).
                while (tmo--) { /* nop */ }
                drv_led_establecer(led_id, LED_OFF);
                return;
            }

            //Botón correcto: el usuario ha contestado a tiempo.
            juego_apagar_todos();
            s.estado = JG_EST_ESPERA_ENTRE_LEDS;

            // Aumentamos dificultad: reducimos tiempos poco a poco.
            if (s.tiempo_respuesta_ms > JUEGO_TIEMPO_MIN_MS + JUEGO_TIEMPO_STEP_MS) {
                s.tiempo_respuesta_ms -= JUEGO_TIEMPO_STEP_MS;
            }
            if (s.delay_entre_ms > JUEGO_DELAY_ENTRE_MIN_MS + JUEGO_DELAY_ENTRE_STEP_MS) {
                s.delay_entre_ms -= JUEGO_DELAY_ENTRE_STEP_MS;
            }

            //Programar tiempo de espera hasta nueva ronda.
            juego_reprogramar(s.delay_entre_ms);
            break;


        case ev_JUEGO_TICK: //Llega un tick del juego.
            
            if (s.num_pares == 0) return;

            if (s.estado == JG_EST_LED_ENCENDIDO){ //El jugador no ha pulsado el botón a tiempo.
                juego_apagar_todos();
                s.estado = JG_EST_ESPERA_ENTRE_LEDS;

                //Programar tiempo de espera hasta nueva ronda.
                juego_reprogramar(s.delay_entre_ms);

            } else if (s.estado == JG_EST_ESPERA_ENTRE_LEDS || s.estado == JG_EST_APAGADO){ //Toca nueva ronda del juego, encendemos nuevo led.

                //Encendemos el nuevo led.
                s.led_actual = juego_siguiente_led(s.led_actual);
                juego_encender_led(s.led_actual);
                s.estado = JG_EST_LED_ENCENDIDO;

                //Tiempo (ya reducido) para que el usuario pulse.
                juego_reprogramar(s.tiempo_respuesta_ms);
            }
            break;

        default:
            // Otros eventos no nos interesan
            break;
    }
}

