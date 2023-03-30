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


axis_data_t mag_data;
void lis_init(){

}

void lis_loop(){

}

void lis_task(void* pvParameters){
 	TickType_t last_wake_time;
	xSemaphoreTake(iic2Mutex,0xFFFF);
	lis_init();
	xSemaphoreGive(iic2Mutex);
	vTaskDelay(5);
	while(1){
		xSemaphoreTake(iic2Mutex,0xFFFF);
		last_wake_time = xTaskGetTickCount();
		//get data from the magnetometer here
		xSemaphoreGive(iic2Mutex);


		vTaskDelayUntil(&last_wake_time, 50);
	}
}


