/*
 * uart_sending_task.c
 *
 *  Created on: Mar 7, 2023
 *      Author: wx
 */

#include "rtos_incs.h"
#include "usart.h"
#include "uart1_task.h"

extern axis_data_t g_accel_data;
extern axis_data_t g_gyro_data;
extern axis_data_t g_mag_data;
extern hts_data_t g_temphum_data;
extern lps_data_t g_pressure_data;
extern warship_state_e g_warship_state;

void uart1_task(void* pvParameters){
	//init code
	char tx_buffer[256];
	uint8_t tx_len;
 	TickType_t delay_time = 10;
 	TickType_t start_time;
	while(1){
		start_time = xTaskGetTickCount();
		switch(g_warship_state){
			case RESCUE:
				tx_len = snprintf(tx_buffer, 256, 			"G: X:%.4fdps | Y:%.4fdps | Z:%.4fdps \r\n",
									g_gyro_data.x, g_gyro_data.y, g_gyro_data.z);
				tx_len += snprintf(tx_buffer+tx_len, 256,	 "A: X:%.4fg  | Y:%.4fg   | Z:%.4fg \r\n",
									g_accel_data.x, g_accel_data.y, g_accel_data.z);
				tx_len += snprintf(tx_buffer+tx_len, 256, 	"P: %.4fpa    | H:%.4f	  | T: %.4f \r\n\n",
									g_pressure_data.pressure, g_temphum_data.humidity, g_temphum_data.temperature);
				tx_len += snprintf(tx_buffer+tx_len, 256, 	"Mode:%d	\r\n\n",
									g_warship_state);
				delay_time = 100;
				break;
			case BATTLE:
				tx_len = snprintf(tx_buffer, 256, 			"G: X:%.4fdps | Y:%.4fdps | Z:%.4fdps \r\n",
									g_gyro_data.x, g_gyro_data.y, g_gyro_data.z);
				tx_len += snprintf(tx_buffer+tx_len, 256,	 "A: X:%.4fg  | Y:%.4fg   | Z:%.4fg \r\n",
									g_accel_data.x, g_accel_data.y, g_accel_data.z);
				tx_len += snprintf(tx_buffer+tx_len, 256, 	"P: %.4fpa    | H:%.4f	  | T: %.4f \r\n\n",
									g_pressure_data.pressure, g_temphum_data.humidity, g_temphum_data.temperature);
				tx_len += snprintf(tx_buffer+tx_len, 256, 	"Mode:%d	\r\n\n",
									g_warship_state);
				delay_time = 100;
				break;
			case BATTLE_WARNING:
				tx_len = snprintf(tx_buffer, 256, 			"G: X:%.4fdps | Y:%.4fdps | Z:%.4fdps \r\n",
									g_gyro_data.x, g_gyro_data.y, g_gyro_data.z);
				tx_len += snprintf(tx_buffer+tx_len, 256,	 "A: X:%.4fg  | Y:%.4fg   | Z:%.4fg \r\n",
									g_accel_data.x, g_accel_data.y, g_accel_data.z);
				tx_len += snprintf(tx_buffer+tx_len, 256, 	"P: %.4fpa    | H:%.4f	  | T: %.4f \r\n",
									g_pressure_data.pressure, g_temphum_data.humidity, g_temphum_data.temperature);
				tx_len += snprintf(tx_buffer+tx_len, 256, 	"Mode:%d	\r\n\n",
									g_warship_state);
				delay_time = 100;
				break;
			default:
				delay_time = 100;
				break;


		}


		HAL_UART_Transmit_DMA(&huart1, (uint8_t*)tx_buffer,tx_len);
		vTaskDelayUntil(&start_time,delay_time);
	}
}
