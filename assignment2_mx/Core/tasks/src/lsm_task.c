/*
 * lsm_task.c
 *
 *  Created on: Mar 7, 2023
 *      Author: wx
 */

#include "rtos_incs.h"
#include "stm32l475e_iot01_accelero.h"
#include "stm32l475e_iot01_gyro.h"
#include "lsm_task.h"

axis_data_t accel_data;
axis_data_t gyro_data;
extern hts_data_t temphum_data;

void lsm_init(){
	BSP_ACCELERO_Init();
	BSP_GYRO_Init();
	//LSM init code hereee
}


void lsm_task(void* argument){
 	TickType_t last_wake_time;
	xSemaphoreTake(iic2Mutex,0xFFFF);
	lsm_init();
	//INSERT INIT CODE HERE
	xSemaphoreGive(iic2Mutex);
	vTaskDelay(5);

	char tx_buffer[256];
	int tx_len;
	int16_t accel_data_i16[3] = { 0 };
	float accel_data[3] = {0.5, 0.5, 0.5};			// array to store the x, y and z readings.
	while(1){
		xSemaphoreTake(iic2Mutex,0xFFFF);
		last_wake_time = xTaskGetTickCount();
		BSP_ACCELERO_AccGetXYZ(accel_data_i16);		// read accelerometer
		// the function above returns 16 bit integers which are acceleration in mg (9.8/1000 m/s^2).
//		// Converting to float to print the actual acceleration.
		xSemaphoreGive(iic2Mutex);


		accel_data[0] = (float)accel_data_i16[0] * (9.8/1000.0f);
		accel_data[1] = (float)accel_data_i16[1] * (9.8/1000.0f);
		accel_data[2] = (float)accel_data_i16[2] * (9.8/1000.0f);
		vTaskDelayUntil(&last_wake_time, 50);
	}
}



