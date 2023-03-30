
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


lps_data_t pressure_data;

void lps_init(){

}

void lps_loop(){

}

void lps_task(void* argument){
 	TickType_t last_wake_time;
	xSemaphoreTake(iic2Mutex,0xFFFF);
	//INSERT INIT CODE HERE
	lps_init();
	xSemaphoreGive(iic2Mutex);
	vTaskDelay(5);
	while(1){
		xSemaphoreTake(iic2Mutex,0xFFFF);
		last_wake_time = xTaskGetTickCount();
		//get data from LPS
		xSemaphoreGive(iic2Mutex);


		vTaskDelayUntil(&last_wake_time, 50);
	}
}
