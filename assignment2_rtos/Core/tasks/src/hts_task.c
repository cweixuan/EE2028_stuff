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
	//block until ready
 	TickType_t last_wake_time;
	xSemaphoreTake(iic2Mutex,0xFFFF);
	//INSERT INIT CODE HERE
	xSemaphoreGive(iic2Mutex);
	vTaskDelay(5);
	while(1){

		xSemaphoreTake(iic2Mutex,0xFFFF);
		last_wake_time = xTaskGetTickCount();
		//INSERT BSP READ HERE
//		float temp = BSP_TSENSOR_ReadTemp();
//		float humidity = BSP_HSENSOR_ReadHumidity();
		xSemaphoreGive(iic2Mutex);

//		temphum_data.temperature = temp;
//		temphum_data.humidity = humidity;

		vTaskDelayUntil(&last_wake_time, 50);
	}
}


