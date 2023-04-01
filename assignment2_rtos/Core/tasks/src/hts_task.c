/*
 * hts_task.c
 *
 *  Created on: Mar 7, 2023
 *      Author: Clinton
 */

#include "rtos_incs.h"
#include "stm32l475e_iot01_tsensor.h"
#include "stm32l475e_iot01_hsensor.h"
#include "stdio.h"
#include "hts_task.h"


hts_data_t g_temphum_data;

void hts_notif(){

}

void hts_init(){
	BSP_HSENSOR_Init();
	BSP_TSENSOR_Init();
}


//VALUES MUST BE RECALIBRATED FOR EVERY SENSOR IF FACTORY CALIBRATION
//IS UNRELIABLE (like ours)
//for wx's board:
#define HUM_H1 (int16_t)76
#define HUM_H1_OUT	(int16_t)32218
#define HUM_H0 (int16_t)66
#define HUM_H0_OUT (int16_t) 32365

#define TEMP_T1 (float)30.0
#define TEMP_T0 (float)28.5
#define TEMP_T1_OUT (int16_t)749
#define TEMP_T0_OUT (int16_t)545


//with custom humidity values because the factory calibration is f'd
//values calibrated with a random clock that measures humidity, lol
//done in an environment chamber....aka a room with aircon
//excuse to turn on aircon :D
float read_humidity(){
	int16_t H_T_out;
	uint8_t buffer[2];
	float tmp_f;
	SENSOR_IO_ReadMultiple(HTS221_I2C_ADDRESS, (HTS221_HR_OUT_L_REG | 0x80), buffer, 2);
	H_T_out = (((uint16_t)buffer[1]) << 8) | (uint16_t)buffer[0];
	tmp_f = (float)(H_T_out - HUM_H0_OUT) * (float)(HUM_H1 - HUM_H0) /
			(float)(HUM_H1_OUT - HUM_H0_OUT)  +  HUM_H0;
	tmp_f = ( tmp_f > 100.0f ) ? 100.0f
		  : ( tmp_f <  0.0f ) ?   0.0f
		  : tmp_f;

	return tmp_f;
}

//recalibrated temperature sensor, reference sensor was a multimeter
//with a K-type probe placed next to the sensor and left to steady for 5s lol.
//done in an environment chamber....aka a room with aircon
//excuse to turn on aircon :D
float read_temp(){
	  int16_t T_out;
	  uint8_t buffer[2];
	  float tmp_f;
	  SENSOR_IO_ReadMultiple(HTS221_I2C_ADDRESS, (HTS221_TEMP_OUT_L_REG | 0x80), buffer, 2);
	  T_out = (((uint16_t)buffer[1]) << 8) | (uint16_t)buffer[0];
  	  tmp_f = (float)(T_out - TEMP_T0_OUT) * (float)(TEMP_T1 - TEMP_T0) /
			  (float)(TEMP_T1_OUT - TEMP_T0_OUT)  +  TEMP_T0;
	  return tmp_f;
}

void hts_task(void* pvParameters){
	//block until ready
 	TickType_t last_wake_time;
	xSemaphoreTake(iic2Mutex,0xFFFF);
	hts_init();                                 	    // init
	xSemaphoreGive(iic2Mutex);
	vTaskDelay(5);
	while(1){
		xSemaphoreTake(iic2Mutex,0xFFFF);
		last_wake_time = xTaskGetTickCount();
		float temp = read_temp();	    	// reading temperature sensor
		float humidity = read_humidity();    // reading humidity sensor
		xSemaphoreGive(iic2Mutex);

		g_temphum_data.temperature = temp;
		g_temphum_data.humidity = (uint8_t)humidity;

		vTaskDelayUntil(&last_wake_time, 2000);
	}
}


