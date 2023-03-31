/*
 * lsm_task.c
 *
 *  Created on: Mar 7, 2023
 *      Author: Clinton
 */

#include "rtos_incs.h"
#include "stm32l475e_iot01_accelero.h"
#include "stm32l475e_iot01_gyro.h"
#include "lsm_task.h"

axis_data_t g_accel_data;
axis_data_t g_gyro_data;

void lsm_init(){
	BSP_ACCELERO_Init();
	BSP_GYRO_Init();
}


void lsm_task(void* argument){
 	TickType_t last_wake_time;
	xSemaphoreTake(iic2Mutex,0xFFFF);
	lsm_init();                                 	    // init
	xSemaphoreGive(iic2Mutex);
	vTaskDelay(5);
	int16_t accel_data_i16[3] = { 0 };		        	// array to store accel x, y, z readings
	float gyro_data_f32[3] = { 0 };		        	// array to store gyro x, y, z readings
	while(1){
		xSemaphoreTake(iic2Mutex,0xFFFF);
		last_wake_time = xTaskGetTickCount();
		BSP_ACCELERO_AccGetXYZ(accel_data_i16);	    	// reading accelerometer
		// this returns 16 bit integers of acceleration in mg (9.8/1000 m/s^2)
		BSP_GYRO_GetXYZ(gyro_data_f32);			    	// reading gyroscope
		// this returns 16 bit integers of magnetic field in mdps (1/1000 dps)? it was written as mg
		xSemaphoreGive(iic2Mutex);

		// converting to float to obtain the actual acceleration
		g_accel_data.x = (float)accel_data_i16[0] * (9.8/1000.0f);
		g_accel_data.y = (float)accel_data_i16[1] * (9.8/1000.0f);
		g_accel_data.z = (float)accel_data_i16[2] * (9.8/1000.0f);

		// converting to float to obtain the actual angular velocity
		g_gyro_data.x = (float)gyro_data_f32[0] * (1/1000.0f);
		g_gyro_data.y = (float)gyro_data_f32[1] * (1/1000.0f);
		g_gyro_data.z = (float)gyro_data_f32[2] * (1/1000.0f);

		vTaskDelayUntil(&last_wake_time, 50);
	}
}


