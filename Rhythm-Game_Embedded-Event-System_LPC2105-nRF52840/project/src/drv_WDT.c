#include "drv_WDT.h"
#include "hal_WDT.h"
#include "board.h"


void drv_WDT_iniciar(uint32_t timeout_s){
    hal_WDT_iniciar(timeout_s);
}

void drv_WDT_alimentar(void){
    hal_WDT_feed();
}






