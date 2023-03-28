/*
 * hts_task.c
 *
 *  Created on: Mar 7, 2023
 *      Author: wx
 */




#include "rtos_incs.h"
#include "stm32l475e_iot01_tsensor.h"
#include "stdio.h"
#include "hts_task.h"

hts_data_t temphum_data;
void hts_init(){

}

void hts_loop(){

}

void hts_task(void* pvParameters){
	hts_init();
	vTaskDelay(5);
	while(1){
		hts_loop();
		vTaskDelay(5);
	}
}


