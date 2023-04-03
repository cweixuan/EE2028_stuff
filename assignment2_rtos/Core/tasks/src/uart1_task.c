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
extern uint8_t enable_extras;

#define GYRO_THRESHOLD (float)200
#define GYRO_OFFSET 0

#define MAG_THRESHOLD (float)3
#define MAG_OFFSET 	1
#define PRES_THRESHOLD (float)1012
#define PRES_OFFSET	2

#define HUMIDITY_THRESHOLD (uint16_t)100
#define HUMI_OFFSET 3

#define ACCEL_THRESHOLD (float)13
#define ACCEL_OFFSET 4

#define TEMP_THRESHOLD (float)35
#define TEMP_OFFSET 5

uint8_t check_over(){
	uint8_t thresh_flag = 0;
	if (g_gyro_data.mag >= GYRO_THRESHOLD){
		thresh_flag |= 1 << GYRO_OFFSET;
	}
	if (g_mag_data.mag >= MAG_THRESHOLD){

		thresh_flag |= 1 << MAG_OFFSET;

	}
	if (g_pressure_data.pressure >= PRES_THRESHOLD){

		thresh_flag |= 1 << PRES_OFFSET;

	}
	if (g_temphum_data.humidity >= HUMIDITY_THRESHOLD){

		thresh_flag |= 1 << HUMI_OFFSET;

	}
	if (g_accel_data.mag >= ACCEL_THRESHOLD){

		thresh_flag |= 1 << ACCEL_OFFSET;

	}
	if (g_temphum_data.temperature >= TEMP_THRESHOLD){

		thresh_flag |= 1 << TEMP_OFFSET;
	}

	return thresh_flag;
};

void uart_notify_from_isr(){
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    configASSERT( uart1_task_handle != NULL );
	vTaskNotifyGiveFromISR(uart1_task_handle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void uart_notify(){
	xTaskNotifyGive(uart1_task_handle);
}

void clear_screen(uint16_t* tx_len, char* tx_buffer, uint16_t max_len){
	if (enable_extras){
		tx_buffer[*tx_len] = 27; //escape
		*tx_len += 1;
		*tx_len += snprintf(tx_buffer+*tx_len,max_len,"[2J");
		tx_buffer[*tx_len] = 27; //escape
		*tx_len += 1;
		*tx_len += snprintf(tx_buffer+*tx_len,max_len,"[H");
	}
}

#define BUFFER_SIZE 256
void uart1_task(void* pvParameters){
	//init code
	char tx_buffer[BUFFER_SIZE];
	uint16_t tx_len;
 	TickType_t delay_time = 10;
 	TickType_t start_time = 0;
 	TickType_t start_delay_time = 0;
	warship_state_e prev_state = DEAD;
	while(1){
		start_delay_time = xTaskGetTickCount();
		delay_time += (start_delay_time-start_time);
		//so it prints immediately
		ulTaskNotifyTake(pdTRUE, delay_time);
		start_time = xTaskGetTickCount();
		tx_len = 0;
		uint8_t over_flag = 0;
		switch(g_warship_state){
			case RESCUE:
				if (prev_state != RESCUE){
					clear_screen(&tx_len, tx_buffer, BUFFER_SIZE);
					tx_len += snprintf(tx_buffer+tx_len, BUFFER_SIZE-tx_len, "Entering SENTRY mode\r\n");
				}
				over_flag = check_over();
				if (over_flag > 0){
					clear_screen(&tx_len, tx_buffer, BUFFER_SIZE);
					if ((over_flag & (1<<GYRO_OFFSET)) > 0 ){
						tx_len += snprintf(tx_buffer+tx_len, BUFFER_SIZE-tx_len,
								"Gyroscope magnitude currently %fdps, exceeding threshold of %fdps|\r\n",
											g_gyro_data.mag,GYRO_THRESHOLD);
					}
					if ((over_flag & (1<<MAG_OFFSET)) > 0 ){
						tx_len += snprintf(tx_buffer+tx_len, BUFFER_SIZE-tx_len,
								"Magnetometer magnitude currently %fgauss, exceeding threshold of %fgauss!\r\n",
											g_mag_data.mag, MAG_THRESHOLD);
					}
					if ((over_flag & (1<<PRES_OFFSET)) >0 ){
						tx_len += snprintf(tx_buffer+tx_len, BUFFER_SIZE-tx_len,
								"Pressure at %fpa, exceeding threshold of %fpa|\r\n",
											g_pressure_data.pressure, PRES_THRESHOLD);
					}
					if ((over_flag & (1<<HUMI_OFFSET)) >0 ){
						tx_len += snprintf(tx_buffer+tx_len, BUFFER_SIZE-tx_len,
								"Humidity at %d%%RH, exceeding threshold of %d%%RH|\r\n",
											g_temphum_data.humidity, HUMIDITY_THRESHOLD);
					}
				}else
				{
					clear_screen(&tx_len, tx_buffer, BUFFER_SIZE);
					tx_len += snprintf(tx_buffer+tx_len, BUFFER_SIZE-tx_len, 	"|GX: %.4fdps	|GY: %.4fdps 	|GZ: %.4fdps 	| \r\n",
										g_gyro_data.x, g_gyro_data.y, g_gyro_data.z);
					tx_len += snprintf(tx_buffer+tx_len, BUFFER_SIZE-tx_len,	"|MX: %.4fgauss	|MY: %.4fgauss 	|MZ: %.4fgauss 	| \r\n",
										g_mag_data.x, g_mag_data.y, g_mag_data.z);
					tx_len += snprintf(tx_buffer+tx_len, BUFFER_SIZE-tx_len, 	"|P: %.4fpa		|H: %d%%RH\r\n",
										g_pressure_data.pressure, g_temphum_data.humidity);

				}
				prev_state = RESCUE;
				delay_time = 1000;
				break;

			case BATTLE:
				if (prev_state != BATTLE){
					clear_screen(&tx_len, tx_buffer, BUFFER_SIZE);
					tx_len += snprintf(tx_buffer+tx_len, BUFFER_SIZE-tx_len, "Entering BATTLE mode\r\n");
				}
				over_flag = check_over();
				if (over_flag > 0){
					clear_screen(&tx_len, tx_buffer, BUFFER_SIZE);
					if ((over_flag & (1<<TEMP_OFFSET)) >0 ){
						tx_len += snprintf(tx_buffer+tx_len, BUFFER_SIZE-tx_len,
								"Temperature at %fC, exceeding threshold of %fC|\r\n",
											g_temphum_data.temperature, TEMP_THRESHOLD);
					}
					if ((over_flag & (1<<PRES_OFFSET)) >0 ){
						tx_len += snprintf(tx_buffer+tx_len, BUFFER_SIZE-tx_len,
								"Pressure at %fpa, exceeding threshold of %fpa|\r\n",
											g_pressure_data.pressure, PRES_THRESHOLD);
					}
					if ((over_flag & (1<<HUMI_OFFSET)) >0 ){
						tx_len += snprintf(tx_buffer+tx_len, BUFFER_SIZE-tx_len,
								"Humidity at %d%%RH, exceeding threshold of %d%%RH|\r\n",
											g_temphum_data.humidity, HUMIDITY_THRESHOLD);
					}
					if ((over_flag & (1<<ACCEL_OFFSET)) >0 ){
						tx_len += snprintf(tx_buffer+tx_len, BUFFER_SIZE-tx_len,
								"Accelerometer magnitude currently %fms2, exceeding threshold of %fms2|\r\n",
											g_accel_data.mag,ACCEL_THRESHOLD);
					}
					if ((over_flag & (1<<GYRO_OFFSET)) >0 ){
						tx_len += snprintf(tx_buffer+tx_len, BUFFER_SIZE-tx_len,
								"Gyroscope magnitude currently %fdps, exceeding threshold of %fdps|\r\n",
											g_gyro_data.mag,GYRO_THRESHOLD);
					}
					if ((over_flag & (1<<MAG_OFFSET)) >0 ){
						tx_len += snprintf(tx_buffer+tx_len, BUFFER_SIZE-tx_len,
								"Magnetometer magnitude currently %fgauss, exceeding threshold of %fgauss!\r\n",
											g_mag_data.mag, MAG_THRESHOLD);
					}
				}else
				{
					clear_screen(&tx_len, tx_buffer, BUFFER_SIZE);
					tx_len += snprintf(tx_buffer+tx_len, BUFFER_SIZE-tx_len, 	"|GX: %.4fdps	|GY: %.4fdps 	|GZ: %.4fdps 	| \r\n",
										g_gyro_data.x, g_gyro_data.y, g_gyro_data.z);
					tx_len += snprintf(tx_buffer+tx_len, BUFFER_SIZE-tx_len,	"|AX: %.4fms^2 	|AY: %.4fms^2 	|AZ: %.4fms^2 	| \r\n",
										g_accel_data.x, g_accel_data.y, g_accel_data.z);
					tx_len += snprintf(tx_buffer+tx_len, BUFFER_SIZE-tx_len,	"|MX: %.4fgauss	|MY: %.4fgauss 	|MZ: %.4fgauss 	| \r\n",
										g_mag_data.x, g_mag_data.y, g_mag_data.z);
					tx_len += snprintf(tx_buffer+tx_len, BUFFER_SIZE-tx_len, 	"|P: %.4fpa		|H: %d%%RH 		|T: %.4fC 		| \r\n",
										g_pressure_data.pressure, g_temphum_data.humidity, g_temphum_data.temperature);

				}

				prev_state = BATTLE;
				delay_time = 1000;
				break;
			case BATTLE_WARNING:
				if (prev_state != BATTLE_WARNING){
					clear_screen(&tx_len, tx_buffer, BUFFER_SIZE);
					tx_len += snprintf(tx_buffer+tx_len, BUFFER_SIZE-tx_len, "WARNING!\r\n");
				} else {
					tx_len = snprintf(tx_buffer,BUFFER_SIZE,"WARNING!\r\n");
				}
				prev_state = BATTLE_WARNING;
				delay_time = 1000;
				break;
			case DEAD:
				clear_screen(&tx_len, tx_buffer, BUFFER_SIZE);
				tx_len += snprintf(tx_buffer+tx_len,BUFFER_SIZE-tx_len,
						"It's been an honour o7\r\n"
						"LONG LIVE ZION!\r\n");
				if (enable_extras){
					tx_len += snprintf(tx_buffer+tx_len,BUFFER_SIZE-tx_len,
							"I HOPE THE ENEMY LIKES THIS ONE LAST RICKROLL\r\n");
				}
				//blocking mode and max delay to ensure this sends out.
				HAL_UART_Transmit_DMA(&huart1, (uint8_t*)tx_buffer,tx_len);
				prev_state = DEAD;
				delay_time = portMAX_DELAY;
				//last o7
				//stop this task
				vTaskSuspend(uart1_task_handle);
				break;
			default:
				//why is it here
				delay_time = 1000;
				break;


		}


		HAL_UART_Transmit_DMA(&huart1, (uint8_t*)tx_buffer,tx_len);
		vTaskDelay(1);
	}
}
