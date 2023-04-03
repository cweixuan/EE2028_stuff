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
extern warship_state_e g_warship_state;
extern uint8_t enable_extras;

void lsm_init() {
	BSP_ACCELERO_Init();
	BSP_GYRO_Init();
	uint8_t tmp;
	tmp = SENSOR_IO_Read(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW,
			LSM6DSL_ACC_GYRO_INT1_CTRL);

	//enable DRDY for gyro and accelerometer
	tmp |= (0b11);
	SENSOR_IO_Write(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_INT1_CTRL, tmp);

	tmp = SENSOR_IO_Read(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW,
			LSM6DSL_ACC_GYRO_DRDY_PULSE_CFG_G);

	//set drdy to pulse
	tmp |= (0b10000000);
	SENSOR_IO_Write(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_DRDY_PULSE_CFG_G, tmp);

}

void lsm_notify_from_isr() {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	configASSERT(lsm_task_handle != NULL);
	vTaskNotifyGiveFromISR(lsm_task_handle, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void lsm_task(void *argument) {
	TickType_t last_wake_time;
	xSemaphoreTake(iic2Mutex, portMAX_DELAY);
	lsm_init();                                 	    // init
	xSemaphoreGive(iic2Mutex);
	vTaskDelay(5);
	int16_t accel_data_i16[3] = { 0 };	// array to store accel x, y, z readings
	float gyro_data_f32[3] = { 0 };		 // array to store gyro x, y, z readings

	xSemaphoreTake(iic2Mutex, portMAX_DELAY);
	BSP_ACCELERO_AccGetXYZ(accel_data_i16);	    	// reading accelerometer
	// this returns 16 bit integers of acceleration in mg (9.8/1000 m/s^2)
	BSP_GYRO_GetXYZ(gyro_data_f32);			    	// reading gyroscope
	// this returns 16 bit integers of magnetic field in mdps (1/1000 dps)? it was written as mg
	xSemaphoreGive(iic2Mutex);

	last_wake_time = xTaskGetTickCount()-1000;
	while(1){
		if (enable_extras){
			ulTaskNotifyTake(pdTRUE, 5000);
		} else {
			vTaskDelayUntil(&last_wake_time, 1000);
		}
		xSemaphoreTake(iic2Mutex, portMAX_DELAY);
		last_wake_time = xTaskGetTickCount();
		BSP_GYRO_GetXYZ(gyro_data_f32);			    	// reading gyroscope
		// this returns 16 bit integers of magnetic field in mdps (1/1000 dps)? it was written as mg
		BSP_ACCELERO_AccGetXYZ(accel_data_i16);	    	// reading accelerometer
		// this returns 16 bit integers of acceleration in mg (9.8/1000 m/s^2)
		xSemaphoreGive(iic2Mutex);

		// converting to float to obtain the actual acceleration
		g_accel_data.x = (float) accel_data_i16[0] * (9.8 / 1000.0f);
		g_accel_data.y = (float) accel_data_i16[1] * (9.8 / 1000.0f);
		g_accel_data.z = (float) accel_data_i16[2] * (9.8 / 1000.0f);
		g_accel_data.mag = sqrtf(
				(g_accel_data.x * g_accel_data.x
						+ g_accel_data.y * g_accel_data.y
						+ g_accel_data.z * g_accel_data.z));

		// converting to float to obtain the actual angular velocity
		g_gyro_data.x = (float) gyro_data_f32[0] * (1 / 1000.0f);
		g_gyro_data.y = (float) gyro_data_f32[1] * (1 / 1000.0f);
		g_gyro_data.z = (float) gyro_data_f32[2] * (1 / 1000.0f);
		g_gyro_data.mag = sqrtf(
				(g_gyro_data.x * g_gyro_data.x + g_gyro_data.y * g_gyro_data.y
						+ g_gyro_data.z * g_gyro_data.z));
		if (g_warship_state == DEAD) {
			BSP_ACCELERO_DeInit();
			BSP_GYRO_DeInit();
		}
	}
}

