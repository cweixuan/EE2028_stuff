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

#define GYRO_THRESHOLD (float)200
#define GYRO_OFFSET 0

#define MAG_THRESHOLD (float)3
#define MAG_OFFSET 	1
#define PRES_THRESHOLD (float)1010
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


void uart1_task(void* pvParameters){
	//init code
	char tx_buffer[256];
	uint8_t tx_len;
 	TickType_t delay_time = 10;
 	TickType_t start_time;
	warship_state_e prev_state = DEAD;
	while(1){
		start_time = xTaskGetTickCount();
		tx_len = 1;
		tx_buffer[0] = 27; //escape
		tx_len += snprintf(tx_buffer+tx_len,256,"[2J");
		tx_len += 1;
		tx_buffer[tx_len-1] = 27; //escape
		tx_len += snprintf(tx_buffer+tx_len,256,"[H");
		uint8_t over_flag = 0;
		switch(g_warship_state){
			case RESCUE:
				if (prev_state == DEAD){
					tx_len += snprintf(tx_buffer+tx_len, 256, "Entering SENTRY mode\r\n");
				}
				over_flag = check_over();
				if (over_flag > 0){
					if ((over_flag & (1<<GYRO_OFFSET)) > 0 ){
						tx_len += snprintf(tx_buffer+tx_len, 256,
								"Gyroscope magnitude currently %fdps, exceeding threshold of %fdps|\r\n",
											g_gyro_data.mag,GYRO_THRESHOLD);
					}
					if ((over_flag & (1<<MAG_OFFSET)) > 0 ){
						tx_len += snprintf(tx_buffer+tx_len, 256,
								"Magnetometer magnitude currently %fT, exceeding threshold of %fT!\r\n",
											g_mag_data.mag, MAG_THRESHOLD);
					}
					if ((over_flag & (1<<PRES_OFFSET)) >0 ){
						tx_len += snprintf(tx_buffer+tx_len, 256,
								"Pressure at %fpa, exceeding threshold of %fpa|\r\n",
											g_pressure_data.pressure, PRES_THRESHOLD);
					}
					if ((over_flag & (1<<HUMI_OFFSET)) >0 ){
						tx_len += snprintf(tx_buffer+tx_len, 256,
								"Humidity at %d%%RH, exceeding threshold of %d%%RH|\r\n",
											g_temphum_data.humidity, HUMIDITY_THRESHOLD);
					}
				}else
				{
					tx_len += snprintf(tx_buffer+tx_len, 256, 	"|GX: %.4fdps|GY: %.4fdps 	|GZ: %.4fdps 	| \r\n",
										g_gyro_data.x, g_gyro_data.y, g_gyro_data.z);
					tx_len += snprintf(tx_buffer+tx_len, 256,	"|MX: %.4fT	 |MY: %.4fT 		|MZ: %.4fT 		| \r\n",
										g_mag_data.x, g_mag_data.y, g_mag_data.z);
					tx_len += snprintf(tx_buffer+tx_len, 256, 	"|P: %.4fpa	 |H: %d%%RH 		| \r\n",
										g_pressure_data.pressure, g_temphum_data.humidity);
					tx_len += snprintf(tx_buffer+tx_len, 256, 	"Mode:%d \r\n",
										g_warship_state);
				}
				prev_state = RESCUE;
				delay_time = 1000;
				break;

			case BATTLE:
				if (prev_state == RESCUE){
					tx_len += snprintf(tx_buffer+tx_len, 256, "Entering BATTLE mode\r\n");
				}
				over_flag = check_over();
				if (over_flag > 0){
					if ((over_flag & (1<<TEMP_OFFSET)) >0 ){
						tx_len += snprintf(tx_buffer+tx_len, 256,
								"Temperature at %fC, exceeding threshold of %fC|\r\n",
											g_temphum_data.temperature, TEMP_THRESHOLD);
					}
					if ((over_flag & (1<<PRES_OFFSET)) >0 ){
						tx_len += snprintf(tx_buffer+tx_len, 256,
								"Pressure at %fpa, exceeding threshold of %fpa|\r\n",
											g_pressure_data.pressure, PRES_THRESHOLD);
					}
					if ((over_flag & (1<<HUMI_OFFSET)) >0 ){
						tx_len += snprintf(tx_buffer+tx_len, 256,
								"Humidity at %d%%RH, exceeding threshold of %d%%RH|\r\n",
											g_temphum_data.humidity, HUMIDITY_THRESHOLD);
					}
					if ((over_flag & (1<<ACCEL_OFFSET)) >0 ){
						tx_len += snprintf(tx_buffer+tx_len, 256,
								"Accelerometer magnitude currently %fms2, exceeding threshold of %fms2|\r\n",
											g_accel_data.mag,ACCEL_THRESHOLD);
					}
					if ((over_flag & (1<<GYRO_OFFSET)) >0 ){
						tx_len += snprintf(tx_buffer+tx_len, 256,
								"Gyroscope magnitude currently %fdps, exceeding threshold of %fdps|\r\n",
											g_gyro_data.mag,GYRO_THRESHOLD);
					}
					if ((over_flag & (1<<MAG_OFFSET)) >0 ){
						tx_len += snprintf(tx_buffer+tx_len, 256,
								"Magnetometer magnitude currently %fT, exceeding threshold of %fT!\r\n",
											g_mag_data.mag, MAG_THRESHOLD);
					}
				}else
				{
					tx_len += snprintf(tx_buffer+tx_len, 256, 	"|GX: %.4fdps	|GY: %.4fdps 	|GZ: %.4fdps 	| \r\n",
										g_gyro_data.x, g_gyro_data.y, g_gyro_data.z);
					tx_len += snprintf(tx_buffer+tx_len, 256,	"|AX: %.4fms^2 	|AY: %.4fms^2 	|AZ: %.4fms^2 	| \r\n",
										g_accel_data.x, g_accel_data.y, g_accel_data.z);
					tx_len += snprintf(tx_buffer+tx_len, 256,	"|MX: %.4fT		|MY: %.4fT 		|MZ: %.4fT 		| \r\n",
										g_mag_data.x, g_mag_data.y, g_mag_data.z);
					tx_len += snprintf(tx_buffer+tx_len, 256, 	"|P: %.4fpa		|H: %d%%RH 		|T: %.4fC 		| \r\n",
										g_pressure_data.pressure, g_temphum_data.humidity, g_temphum_data.temperature);
					tx_len += snprintf(tx_buffer+tx_len, 256, 	"Mode:%d \r\n",
										g_warship_state);
				}

				prev_state = BATTLE;
				delay_time = 1000;
				break;
			case BATTLE_WARNING:
				if (prev_state == BATTLE){
					tx_len += snprintf(tx_buffer+tx_len, 256, "WARNING!\r\n");
				} else {
					tx_len = snprintf(tx_buffer,256,"WARNING!\r\n");
				}
				prev_state = BATTLE_WARNING;
				delay_time = 1000;
				break;
			default:
				tx_len += snprintf(tx_buffer+tx_len,256, "It's been an honour o7\r\n");
				prev_state = RESCUE;
				delay_time = 0xFFFFFFFF;
				break;


		}


		HAL_UART_Transmit_DMA(&huart1, (uint8_t*)tx_buffer,tx_len);
		vTaskDelayUntil(&start_time,delay_time);
	}
}
