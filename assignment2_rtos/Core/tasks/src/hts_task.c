/*
 * hts_task.c
 *
 *  Created on: Mar 7, 2023
 *      Author: Clinton
 */

#include "rtos_incs.h"
#include "stm32l475e_iot01_tsensor.h"
#include "stm32l475e_iot01_hsensor.h"
#include "hts_task.h"


hts_data_t g_temphum_data;
extern warship_state_e g_warship_state;
extern uint8_t enable_extras;

void hts_notif(){

}

void hts_init(){
	BSP_TSENSOR_Init();
	uint8_t tmp;
	  //enable DRDY for hts221
	  tmp = SENSOR_IO_Read(HTS221_I2C_ADDRESS, HTS221_CTRL_REG3);
	  //set DRDY pin to pushpull configuration and active high
	  tmp &= ~((1 << HTS221_PP_OD_BIT) | (1 << HTS221_DRDY_H_L_BIT));
	  //enable DRDY pin
	  tmp |= (1 << HTS221_DRDY_BIT);
	  SENSOR_IO_Write(HTS221_I2C_ADDRESS, HTS221_CTRL_REG3, tmp);

}

//function to notify the task from the EXTI interrupt
void hts_notify_from_isr(){
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    configASSERT( hts_task_handle != NULL );
	vTaskNotifyGiveFromISR(hts_task_handle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}


//VALUES MUST BE RECALIBRATED FOR EVERY SENSOR IF FACTORY CALIBRATION
//IS UNRELIABLE (like ours)
//for wx's board:
#define HUM_H1 (int16_t)78
#define HUM_H1_OUT	(int16_t)32225
#define HUM_H0 (int16_t)59
#define HUM_H0_OUT (int16_t) 32443

#define TEMP_T1 (float)29.9
#define TEMP_T0 (float)28.5
#define TEMP_T1_OUT (int16_t)674
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
	xSemaphoreTake(iic2Mutex,portMAX_DELAY);
	hts_init();                                 	    // init
	xSemaphoreGive(iic2Mutex);

	vTaskDelay(5);
	//clear the DRDY pin which might be on right now, flag might be better?
	xSemaphoreTake(iic2Mutex,portMAX_DELAY);
	BSP_TSENSOR_ReadTemp();	    	// reading temperature sensor
	read_humidity();    // reading humidity sensor
	xSemaphoreGive(iic2Mutex);
	last_wake_time = xTaskGetTickCount()-1000;
	while(1){
		if (enable_extras){
			ulTaskNotifyTake(pdFALSE, 5000);
		} else {
			vTaskDelayUntil(&last_wake_time, 1000);
		}
		xSemaphoreTake(iic2Mutex,portMAX_DELAY);
		last_wake_time = xTaskGetTickCount();
		float temp = BSP_TSENSOR_ReadTemp();	    	// reading temperature sensor
		float humidity = read_humidity();    // reading humidity sensor
		xSemaphoreGive(iic2Mutex);

		g_temphum_data.temperature = temp;
		g_temphum_data.humidity = (uint8_t)humidity;
	}
}


