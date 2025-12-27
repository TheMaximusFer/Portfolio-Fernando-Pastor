#include "drv_monitor.h"
#include "hal_gpio.h"
#include "board.h"

#if MONITOR_NUMBER > 0
	static const HAL_GPIO_PIN_T s_monitor_list[MONITOR_NUMBER] = MONITOR_LIST;
#endif

uint32_t drv_monitor_iniciar(void) {
	
	#if MONITOR_NUMBER > 0
		for (MONITOR_id_t i = 1; i <= (MONITOR_id_t)MONITOR_NUMBER; ++i){
				hal_gpio_sentido(s_monitor_list[i-1], HAL_GPIO_PIN_DIR_OUTPUT);
		}

    return (uint32_t)MONITOR_NUMBER; 
	#else
	return (uint32_t)MONITOR_NUMBER; 
	#endif	
}


void drv_monitor_marcar(uint32_t id) {
		#if MONITOR_NUMBER > 0
			 hal_gpio_escribir(s_monitor_list[id-1], MONITOR_ON);
		#endif
}

void drv_monitor_desmarcar(uint32_t id) {
    
		#if MONITOR_NUMBER > 0
			 hal_gpio_escribir(s_monitor_list[id-1], MONITOR_OFF);
		#endif
}
