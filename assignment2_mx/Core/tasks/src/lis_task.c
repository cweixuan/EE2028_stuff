/*
 * accel_rtos_task.c
 *
 *  Created on: Mar 7, 2023
 *      Author: wx
 */

#include "rtos_incs.h"
#include "../../Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_magneto.h"
#include "stdio.h"
#include "../../tasks/inc/lis_task.h"



void lis_init(){

}

void lis_loop(){

}

void lis_task(void* pvParameters){
	vTaskDelay(5);
	uint8_t toggle = 0;
	while(1){
		vTaskDelay(5);
		toggle = !toggle;
	}
}


