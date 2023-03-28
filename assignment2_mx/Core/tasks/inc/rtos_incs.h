/*
 * rtos_vars.h
 *
 *  Created on: Mar 7, 2023
 *      Author: wx
 */

#ifndef INC_RTOS_INCS_H_
#define INC_RTOS_INCS_H_

#include "main.h"
#include "stdio.h"
#include "cmsis_os.h"
#include "string.h"
#include "typedefs.h"
extern TaskHandle_t uart1_task_handle;
extern TaskHandle_t lis_task_handle;
extern TaskHandle_t lps_task_handle;
extern TaskHandle_t lsm_task_handle;
extern TaskHandle_t hts_task_handle;

//extern axis_data_t mag_data;
//extern axis_data_t accel_data;
//extern axis_data_t gyro_data;
//extern lps_data_t pressure_data;
//extern hts_data_t temphum_data;

//struct for axis data, z points down, x is facing USB connector
typedef struct {
	float x;
	float y;
	float z;
}axis_data_t;

typedef struct{
	float pressure;
}lps_data_t;

typedef struct{
	float temperature;
	float humidity;
}hts_data_t;


#endif /* INC_RTOS_INCS_H_ */
