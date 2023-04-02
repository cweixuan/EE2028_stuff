/*
 * lps_task.c
 *
 *  Created on: Mar 7, 2023
 *      Author: Clinton
 */

#include "rtos_incs.h"
#include "stm32l475e_iot01_psensor.h"
#include "lps_task.h"


lps_data_t g_pressure_data;
extern warship_state_e g_warship_state;
extern uint8_t enable_extras;

void lps_init(){
	BSP_PSENSOR_Init();
}

void lps_notify_from_isr(){
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    configASSERT( lps_task_handle != NULL );
	vTaskNotifyGiveFromISR(&lps_task_handle, xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void lps_task(void* argument){
 	TickType_t last_wake_time;
	xSemaphoreTake(iic2Mutex,portMAX_DELAY);
	lps_init();                                 	    // init
	xSemaphoreGive(iic2Mutex);
	vTaskDelay(5);
	//clear interrupts before running task
	xSemaphoreTake(iic2Mutex,portMAX_DELAY);
	BSP_PSENSOR_ReadPressure();		// reading pressure sensor
	xSemaphoreGive(iic2Mutex);
	while(1){
		ulTaskNotifyTake(pdTRUE, 2000);
		xSemaphoreTake(iic2Mutex,portMAX_DELAY);
		last_wake_time = xTaskGetTickCount();
		float pressure_data = BSP_PSENSOR_ReadPressure();		// reading pressure sensor
		xSemaphoreGive(iic2Mutex);
		g_pressure_data.pressure = pressure_data;
	}
}
