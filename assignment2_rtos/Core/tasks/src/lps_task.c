/*
 * lps_task.c
 *
 *  Created on: Mar 7, 2023
 *      Author: Clinton
 */

#include "rtos_incs.h"
#include "stm32l475e_iot01_psensor.h"
#include "stdio.h"
#include "lps_task.h"


lps_data_t g_pressure_data;

void lps_init(){
	BSP_PSENSOR_Init();
}

void lps_loop(){

}

void lps_task(void* argument){
 	TickType_t last_wake_time;
	xSemaphoreTake(iic2Mutex,0xFFFF);
	lps_init();                                 	    // init
	xSemaphoreGive(iic2Mutex);
	vTaskDelay(5);
	while(1){
		xSemaphoreTake(iic2Mutex,0xFFFF);
		last_wake_time = xTaskGetTickCount();
		float pressure_data = BSP_PSENSOR_ReadPressure();		// reading pressure sensor
		xSemaphoreGive(iic2Mutex);
		g_pressure_data.pressure = pressure_data;

		vTaskDelayUntil(&last_wake_time, 50);
	}
}
