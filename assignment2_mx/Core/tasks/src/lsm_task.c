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

void lsm_init(){

}

void lsm_loop(){

}

void lsm_task(void* argument){
	BSP_ACCELERO_Init();
	char tx_buffer[256];
	int tx_len;
	int16_t accel_data_i16[3] = { 0 };
	float accel_data[3];			// array to store the x, y and z readings.
	while(1){
		TickType_t last_wake_time = xTaskGetTickCount();
		BSP_ACCELERO_AccGetXYZ(accel_data_i16);		// read accelerometer
		// the function above returns 16 bit integers which are acceleration in mg (9.8/1000 m/s^2).
		// Converting to float to print the actual acceleration.
		accel_data[0] = (float)accel_data_i16[0] * (9.8/1000.0f);
		accel_data[1] = (float)accel_data_i16[1] * (9.8/1000.0f);
		accel_data[2] = (float)accel_data_i16[2] * (9.8/1000.0f);
		tx_len = sprintf((char*)tx_buffer,"AccelX : %f; Accel Y: %f; Accel Z: %f\n", accel_data[0], accel_data[1], accel_data[2]);
		queue_UART_msg_t accelero_send;
		memcpy(accelero_send.uart_chars, tx_buffer, tx_len);
		accelero_send.char_len = tx_len;
		accelero_send.type = TX;
//		xQueueSendToBack(UART1_queue,&accelero_send,10);
		vTaskDelayUntil(&last_wake_time, 1000);
	}
}



