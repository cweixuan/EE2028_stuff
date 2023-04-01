/*
 * accel_rtos_task.c
 *
 *  Created on: Mar 7, 2023
 *      Author: Clinton
 */

#include "rtos_incs.h"
#include "stm32l475e_iot01_magneto.h"
#include "stdio.h"
#include "arm_fp16.h"
#include "lis_task.h"


axis_data_t g_mag_data;

void lis_init(){
	BSP_MAGNETO_Init();
}

void lis_task(void* pvParameters){
 	TickType_t last_wake_time;
	xSemaphoreTake(iic2Mutex,0xFFFF);
	lis_init();                             	        // init
	xSemaphoreGive(iic2Mutex);
	vTaskDelay(5);
	int16_t mag_data_i16[3] = { 0 };		        	// array to store mag x, y, z readings
	while(1){
		xSemaphoreTake(iic2Mutex,0xFFFF);
		last_wake_time = xTaskGetTickCount();
		BSP_MAGNETO_GetXYZ(mag_data_i16);	        	// reading magnetometer
		// this returns 16 bit integers of magnetic field in mGauss (1/1000 Gauss)
		xSemaphoreGive(iic2Mutex);

		// converting to float to obtain the actual magnetic field
		g_mag_data.x = (float)mag_data_i16[0] * (1/1000.0f);
		g_mag_data.y = (float)mag_data_i16[1] * (1/1000.0f);
		g_mag_data.z = (float)mag_data_i16[2] * (1/1000.0f);
		g_mag_data.mag = sqrtf((g_mag_data.x*g_mag_data.x +
								g_mag_data.y * g_mag_data.y +
								g_mag_data.z * g_mag_data.z));

		vTaskDelayUntil(&last_wake_time, 50);
	}
}


