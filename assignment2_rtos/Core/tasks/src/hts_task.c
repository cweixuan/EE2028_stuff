/*
 * hts_task.c
 *
 *  Created on: Mar 7, 2023
 *      Author: Clinton
 */

#include "rtos_incs.h"
#include "stm32l475e_iot01_tsensor.h"
#include "stm32l475e_iot01_hsensor.h"
#include "stdio.h"
#include "hts_task.h"


hts_data_t g_temphum_data;

void hts_init(){
	BSP_TSENSOR_Init();
	BSP_HSENSOR_Init();
}

void hts_loop(){

}

void hts_task(void* pvParameters){
	//block until ready
 	TickType_t last_wake_time;
	xSemaphoreTake(iic2Mutex,0xFFFF);
	hts_init();                                 	    // init
	xSemaphoreGive(iic2Mutex);
	vTaskDelay(5);
	while(1){
		xSemaphoreTake(iic2Mutex,0xFFFF);
		last_wake_time = xTaskGetTickCount();
		float temp = BSP_TSENSOR_ReadTemp();	    	// reading temperature sensor
		float humidity = BSP_HSENSOR_ReadHumidity();    // reading humidity sensor
		xSemaphoreGive(iic2Mutex);

		g_temphum_data.temperature = temp;
		g_temphum_data.humidity = humidity;

		vTaskDelayUntil(&last_wake_time, 2000);
	}
}


