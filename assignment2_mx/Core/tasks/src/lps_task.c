
/*
 * lps_task.c
 *
 *  Created on: Mar 7, 2023
 *      Author: wx
 */


#include "rtos_incs.h"
#include "stm32l475e_iot01_psensor.h"
#include "stdio.h"
#include "lps_task.h"
void lps_init(){

}

void lps_loop(){

}

void lps_task(void* argument){
	lps_init();
	vTaskDelay(5);
	while(1){
		lps_loop();
		vTaskDelay(5);
	}
}
