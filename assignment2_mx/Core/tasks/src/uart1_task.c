/*
 * uart_sending_task.c
 *
 *  Created on: Mar 7, 2023
 *      Author: wx
 */

#include "rtos_incs.h"
#include "usart.h"
#include "uart1_task.h"

extern axis_data_t accel_data;
extern axis_data_t gyro_data;
extern axis_data_t mag_data;
extern hts_data_t temphum_data;
extern lps_data_t pressure_data;
extern warship_state_e warship_state;

void uart1_task(void* pvParameters){
	//init code
	char tx_buffer[256];
	uint8_t tx_len;
 	TickType_t delay_time = 1;
 	TickType_t start_time;
	while(1){
		start_time = xTaskGetTickCount();
		switch(warship_state){
			case SENTRY:
				tx_len = snprintf(tx_buffer, 256, "G: X:%.4fdps | Y:%.4fdps | Z:%.4fdps \r\n",
									gyro_data.x, gyro_data.y, gyro_data.z);
				tx_len += snprintf(tx_buffer+tx_len, 256, "A: X:%.4fg | Y:%.4fg | Z:%.4fg \r\n",
									accel_data.x, accel_data.y, accel_data.z);
				tx_len += snprintf(tx_buffer+tx_len, 256, "P: %.4fpa | H:%.4f\r\n",
									pressure_data.pressure, temphum_data.humidity);
				break;
			case WARNING:
				tx_len = snprintf(tx_buffer, 256, "G: X:%.4fdps | Y:%.4fdps | Z:%.4fdps \r\n",
									gyro_data.x, gyro_data.y, gyro_data.z);
				tx_len += snprintf(tx_buffer+tx_len, 256, "A: X:%.4fg | Y:%.4fg | Z:%.4fg \r\n",
									accel_data.x, accel_data.y, accel_data.z);
				tx_len += snprintf(tx_buffer+tx_len, 256, "P: %.4fpa | H:%.4f\r\n",
									pressure_data.pressure, temphum_data.humidity);
				break;
			case BATTLE:
				tx_len = snprintf(tx_buffer, 256, "G: X:%.4fdps | Y:%.4fdps | Z:%.4fdps \r\n",
									gyro_data.x, gyro_data.y, gyro_data.z);
				tx_len += snprintf(tx_buffer+tx_len, 256, "A: X:%.4fg | Y:%.4fg | Z:%.4fg \r\n",
									accel_data.x, accel_data.y, accel_data.z);
				tx_len += snprintf(tx_buffer+tx_len, 256, "P: %.4fpa | H:%.4f\r\n",
									pressure_data.pressure, temphum_data.humidity);
				break;
			default:
				delay_time = 0;
				break;


		}


		HAL_UART_Transmit_DMA(&huart1, tx_buffer,tx_len);
		vTaskDelayUntil(start_time,delay_time);
	}
}
