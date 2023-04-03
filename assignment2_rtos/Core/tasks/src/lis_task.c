/*
 * accel_rtos_task.c
 *
 *  Created on: Mar 7, 2023
 *      Author: Clinton
 */

#include "rtos_incs.h"
#include "stm32l475e_iot01_magneto.h"
#include "lis_task.h"


axis_data_t g_mag_data;
extern warship_state_e g_warship_state;
extern uint8_t enable_extras;

void lis_init(){
	BSP_MAGNETO_Init();
	uint8_t tmp;
	tmp = SENSOR_IO_Read(LIS3MDL_MAG_I2C_ADDRESS_LOW,
			LIS3MDL_MAG_INT_CFG);
	//disable all threshold interupts, set interrupt to active high, enable interrupt
	tmp = (0b00001101);
	//enable DRDY for gyro and accelerometer
	SENSOR_IO_Write(LIS3MDL_MAG_I2C_ADDRESS_LOW, LIS3MDL_MAG_INT_CFG, tmp);

}

void lis_notify_from_isr(){
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    configASSERT( lis_task_handle != NULL );
	vTaskNotifyGiveFromISR(lis_task_handle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void lis_task(void* pvParameters){
 	TickType_t last_wake_time;
	xSemaphoreTake(iic2Mutex,portMAX_DELAY);
	lis_init();                             	        // init
	xSemaphoreGive(iic2Mutex);
	vTaskDelay(5);
	int16_t mag_data_i16[3] = { 0 };		        	// array to store mag x, y, z readings

	xSemaphoreTake(iic2Mutex,portMAX_DELAY);
	BSP_MAGNETO_GetXYZ(mag_data_i16);	        	// reading magnetometer
	// this returns 16 bit integers of magnetic field in mGauss (1/1000 Gauss)
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

	}
}


