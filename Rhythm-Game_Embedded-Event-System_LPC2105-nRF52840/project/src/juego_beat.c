#include "juego_beat.h"
#include "drv_leds.h"
#include "svc_alarmas.h"
#include "drv_tiempo.h"
#include "board.h"
#include "drv_consumo.h"
#include "drv_random.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

// ===========================
//   CONSTANTES
// ===========================
#define JUEGO_BPM_INICIAL             60
#define JUEGO_COMPASES_MAX            30
#define JUEGO_PUNTOS_MIN              -5
#define JUEGO_VENTANA_PORC            40
#define JUEGO_PRECISION_10            10
#define JUEGO_PRECISION_20            20
#define JUEGO_COMPASES_POR_NIVEL      7
#define JUEGO_TIMEOUT_INACTIVIDAD_MS  10000
#define JUEGO_NIVEL_MAX               4

#define ANIM_PARPADEO_MS      500
#define ANIM_FIN_MS           300

// ------------------- Estado interno -------------------------
typedef enum {
    INICIO = 0,             
    MOSTRAR_SECUENCIA = 1,  
    ESPERAR_ENTRADA = 2,
		VERIFICANDO_SALIDA = 3,	
    FIN_PARTIDA = 4,        
    DORMIR = 5              
} juego_estado_t;

typedef struct {
    juego_estado_t estado;
    uint8_t compas[3];          
    uint8_t nivel;              
    uint8_t bpm;                
    uint32_t t_compas_ms;       
    uint32_t ventana_ms;        
                 
    int8_t compases_jugados;   
    
    uint32_t timestamp_inicio;  
    uint8_t patron_recibido;    
    bool primera_pulsacion;     
    uint32_t timestamp_primera_pulsacion; 

    uint8_t paso_animacion;     
    bool anim_exito;     

		//-----Estadísticas-------
		int32_t puntuacion;
		uint8_t puntuacion_por_compas;
		uint8_t puntuacion_por_nivel[4];
		uint32_t tiempo_reaccion_total;
		uint32_t tiempo_reaccion_medio_ms;
		uint32_t tiempo_reaccion_mas_rapido_ms;
} juego_t;

static juego_t j; 

// --------------- Helpers -------------------

static void juego_apagar_todos(void){
    for (uint8_t i = 1; i <= LEDS_NUMBER; ++i) drv_led_establecer((LED_id_t)i, LED_OFF);
}

static void juego_encender_todos(void){
    for (uint8_t i = 1; i <= LEDS_NUMBER; ++i) drv_led_establecer((LED_id_t)i, LED_ON);
}

static void juego_reprogramar(uint32_t retardo_ms){
    svc_alarma_activar(svc_alarma_codificar(0, retardo_ms, 0), ev_JUEGO_TICK, 0);
}

static void juego_resetear_inactividad(void) {
    svc_alarma_activar(svc_alarma_codificar(0, JUEGO_TIMEOUT_INACTIVIDAD_MS, 0), ev_INACTIVIDAD, 0);
}

// --------------- Lógica  -------------------

static uint8_t generar_compas(void){
		static uint8_t ultimo_generado = 0;
    uint32_t r;
    uint8_t resultado = 0;

    do {
        r = drv_random_generar_rango(4); // Genera 0, 1, 2, 3
        
        switch (j.nivel) {
            case 1: 
                //Nivel 1: SOLO un LED encendido (1 o 2). 
                //No se permiten vacíos (0) ni dobles (3).
                if (r == 0) resultado = 1;
                else if (r == 3) resultado = 2;
                else resultado = (uint8_t)r;
                break;
                
            case 2: 
                //Nivel 2: Se permiten compases sin leds (0).
                //NO se permiten dobles (3).
                if (r == 3) resultado = 0; 
                else resultado = (uint8_t)r;
                break;
                
            default: 
                //Nivel 3 y 4: Permitir dos LEDs (3).
                resultado = (uint8_t)r; 
                break;
        }
        
        //En nivel 1 forzamos alternancia para que sea visualmente claro.
        if (j.nivel == 1 && ultimo_generado != 0 && resultado == ultimo_generado) {
            //Si sale repetido en nivel 1, forzamos el contrario.
            resultado = (resultado == 1) ? 2 : 1; 
        }

    } while (resultado == ultimo_generado && j.nivel > 1); //Evitar repetición exacta en niveles altos.

    ultimo_generado = resultado;
    return resultado; 
}

static void avanzar_compas(void){
    j.compas[0] = j.compas[1];
    j.compas[1] = j.compas[2];
    j.compas[2] = generar_compas();
}

static void mostrar_compases(void){
    juego_apagar_todos();
    //ARRIBA (Leds 1 y 2) .
    if (j.compas[2] & 0x01) drv_led_establecer(1, LED_ON);
    if (j.compas[2] & 0x02) drv_led_establecer(2, LED_ON);

    //ABAJO (Leds 3 y 4).
    if (j.compas[1] & 0x01) drv_led_establecer(3, LED_ON);
    if (j.compas[1] & 0x02) drv_led_establecer(4, LED_ON);
}

static void procesar_pulsacion(uint32_t boton_id) {
    uint32_t ahora = drv_tiempo_actual_ms();
    if (!j.primera_pulsacion) {
        j.primera_pulsacion = true;
        j.timestamp_primera_pulsacion = ahora;
        j.patron_recibido = 0;
    }
    if (boton_id == 1) j.patron_recibido |= 0x01;
    if (boton_id == 2) j.patron_recibido |= 0x02;
}

static void evaluar_ronda(void){
    if (!j.primera_pulsacion) {// Si no pulsó nada...
        if (j.compas[0] != 0) j.puntuacion--;
        return;
    }
		
		uint32_t tiempo = j.timestamp_primera_pulsacion - j.timestamp_inicio;
    
    //Fuera de ventana (>40%): -1 punto.
    if (tiempo > j.ventana_ms) {
        j.puntuacion -= 1;
        return;
    }

    //Patrón incorrecto: -1 punto.
    uint8_t esperado = j.compas[0];
    if (j.patron_recibido != esperado) {
        j.puntuacion -= 1; 
        return;
    }

    //Calcular precisión.
    uint32_t porcentaje = (tiempo * 100) / j.t_compas_ms;

    //Asignación de puntos:
    if (porcentaje <= JUEGO_PRECISION_10) {
        j.puntuacion += 2; //+2 si 10%
    } 
    else if (porcentaje <= JUEGO_PRECISION_20) {
        j.puntuacion += 1; //+1 si 20%
    }
    //0 si del 20 al 40% (no hacemos nada).
		
		// Actualizar estadísticas
		j.puntuacion_por_nivel[j.nivel-1] += j.puntuacion;
		if (j.compases_jugados > 0) j.puntuacion_por_compas = j.puntuacion / j.compases_jugados;
		j.tiempo_reaccion_total += tiempo;
		if (j.compases_jugados > 0) j.tiempo_reaccion_medio_ms = j.tiempo_reaccion_total / j.compases_jugados;
		if (tiempo < j.tiempo_reaccion_mas_rapido_ms) j.tiempo_reaccion_mas_rapido_ms = tiempo;
		
}

static void actualizar_nivel_y_bpm(void){
		//Si se han superado todos los compases del nivel se pasa al siguiente si aún quedan.
    if ((j.compases_jugados % JUEGO_COMPASES_POR_NIVEL) == 0 && j.nivel < JUEGO_NIVEL_MAX)
        j.nivel++;

		if (j.nivel == JUEGO_NIVEL_MAX) j.bpm += 2;   //Aumenta dificultad.

    j.t_compas_ms = 60000 / j.bpm;
    j.ventana_ms = (j.t_compas_ms * JUEGO_VENTANA_PORC) / 100;
}

// ------------------- Interfaz pública -----------------------

void juego_beat_iniciar(void){
    juego_apagar_todos();
    j.nivel = 1;
    j.bpm = JUEGO_BPM_INICIAL;
    j.puntuacion = 0;
		j.puntuacion_por_compas = 0;
		for (uint8_t i=0; i<JUEGO_NIVEL_MAX; i++) j.puntuacion_por_nivel[i] = 0;
	  j.tiempo_reaccion_total = 0;
		j.tiempo_reaccion_medio_ms = 0;
		j.tiempo_reaccion_mas_rapido_ms = j.ventana_ms; //no puede reaccionar más lento que la ventana porque perdería un punto.
    j.compases_jugados = -1; //comienza en -1 porque tiene que bajar el primer pixel hasta abajo para empezar.
    j.t_compas_ms = 60000/ j.bpm; 
		j.patron_recibido = 0;
    j.primera_pulsacion = false;
    j.timestamp_primera_pulsacion = 0;
    
    j.compas[0] = 0; 
    j.compas[1] = 0; 
    j.compas[2] = generar_compas();

    juego_resetear_inactividad();

    j.paso_animacion = 0;
    j.estado = INICIO;
    juego_encender_todos();
    juego_reprogramar(ANIM_PARPADEO_MS);
}

void juego_beat_actualizar(EVENTO_T evento, uint32_t auxData){
    
	  //Si vence el periodo de inactividad se duerme.
    if (evento == ev_INACTIVIDAD && j.estado != DORMIR) {   
				juego_apagar_todos();
				j.estado = DORMIR;
				juego_reprogramar(1); 
        return;
    }
		//Si se detecta una pulsación larga se inicia nueva partida.
    if (evento == ev_PULSACION_LARGA && j.estado != DORMIR) {
        juego_beat_iniciar();
        return;
    }

    switch (j.estado) {
        case INICIO:
            if (evento == ev_JUEGO_TICK) {
								//Realizar animación comienzo partida.
                j.paso_animacion++;
                switch(j.paso_animacion) {
                    case 1: juego_apagar_todos();   juego_reprogramar(ANIM_PARPADEO_MS); break;
                    case 2: juego_encender_todos(); juego_reprogramar(ANIM_PARPADEO_MS); break;
                    case 3: juego_apagar_todos();   juego_reprogramar(200); break; 
                    default: 
												//Cuando acaba la animación se pasa a MOSTRAR_SECUENCIA.
                        j.estado = MOSTRAR_SECUENCIA;
                        juego_reprogramar(1); 
                        break;
                }
            }
            break;

        case MOSTRAR_SECUENCIA:
            if (evento == ev_JUEGO_TICK) {
							  //Se muestra la secuencia de leds al jugador y se pasa a esperar a entrada.
                mostrar_compases();
                j.timestamp_inicio = drv_tiempo_actual_ms();
                j.patron_recibido = 0;
                j.primera_pulsacion = false;
                juego_reprogramar(j.t_compas_ms);
                j.estado = ESPERAR_ENTRADA;
            }
            break;

        case ESPERAR_ENTRADA:
            if (evento == ev_BOTON_PULSAR) {
								//Si llega una pulsación se resetea la inactividad.
                juego_resetear_inactividad(); 
								//Si se pulsa el botón 3 o 4 significa que se acaba la partida o se reinicia
							  //así que se pasa a VERIFICANDO_SALIDA
                if (auxData >= 3) { // Salir
										juego_apagar_todos();
                    j.estado = VERIFICANDO_SALIDA;
                    juego_reprogramar(3100); //más de 3 segundos para que de tiempo a pulsación reinicio
                    return;
                }
								// Si no se procesa la pulsación del botón para luego ver si ha acertado el jugador o no
                if (auxData == 1 || auxData == 2) procesar_pulsacion(auxData);
            
            } else if (evento == ev_JUEGO_TICK) {
								//Cuando vence la reprogramación con el compás se calcula la puntuacion y estadísticas.
								j.compases_jugados++;
                evaluar_ronda();
                avanzar_compas();
								//Actualizar nivel, si procede.
							  actualizar_nivel_y_bpm();
                
							  //Fin de partida si llega al máximo de compases o si hace una mala puntuación.
                if (j.compases_jugados >= JUEGO_COMPASES_MAX || j.puntuacion <= JUEGO_PUNTOS_MIN) { //comentar "|| j.puntuacion <= JUEGO_PUNTOS_MIN)" para no perder por puntos y ver apagado por inactividad.
                    j.estado = FIN_PARTIDA;
										
										if (j.compases_jugados >= JUEGO_COMPASES_MAX) {
                        j.anim_exito = true; //Ganar.
                    } else {
                        j.anim_exito = false; //Perder.
                    }
									  //Se prepara la animación de fin de partida.
                    j.paso_animacion = 0;
                    juego_reprogramar(1);
                } else {
										//Si no ha acabado se pasa a la siguiente ronda/compás.
                    j.estado = MOSTRAR_SECUENCIA;
                    juego_reprogramar(1);
                }
            }
            break;
				case VERIFICANDO_SALIDA:
						//while(1); //descomentar para comprobar el whatchdog
				
            //Si llega el evento de pulsación larga: REINICIAR
            if (evento == ev_PULSACION_LARGA) {
                juego_beat_iniciar();
            }
            //Si se acaba el tiempo (Tick de 3100ms) y no llegó pulsación larga: fin de partida.
            else if (evento == ev_JUEGO_TICK) {
                j.estado = FIN_PARTIDA;
                j.paso_animacion = 0;
                juego_reprogramar(1);
            }
            break;
						
        case FIN_PARTIDA:
            if (evento == ev_JUEGO_TICK) {
								// Mostrar animación
                j.paso_animacion++;
                if (j.paso_animacion < 8) {
                    if (j.paso_animacion % 2 != 0) {
                        //Distinguir animaciones.
                        if (j.anim_exito) {
                            juego_encender_todos(); //VICTORIA, todo luz.
                        } else {
                            //DERROTA:Cruz Leds 1 y 4
                            juego_apagar_todos();
                            drv_led_establecer(1, LED_ON);
                            drv_led_establecer(4, LED_ON);
                        }
                    }
                    else juego_apagar_todos();
                    
                    juego_reprogramar(ANIM_FIN_MS);
                } else {
										//Cuando se acaba la animación pasa a dormirse.
                    j.estado=DORMIR;
                    juego_reprogramar(1);
                }
            }
            break;

        case DORMIR:
						juego_apagar_todos();
            drv_consumo_dormir();
            break;
    }
}
