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
extern TaskHandle_t warship_task_handle;


extern SemaphoreHandle_t iic2Mutex;
extern EventGroupHandle_t changeStateFlag;

//function prototypes for interrupts from the drdy pins of the sensors
void hts_notif();
void lps_notif();
void lsm_notif();
void lis_notif();


//struct for axis data, z points down, x is facing USB connector
typedef struct {
	float x;
	float y;
	float z;
	float mag;
}axis_data_t;

typedef struct{
	float pressure;
}lps_data_t;

typedef struct{
	float temperature;
	uint8_t humidity;
}hts_data_t;


typedef enum {
	RESCUE=0,
	BATTLE=1,
	BATTLE_WARNING=2,
	DEAD = 4
}warship_state_e;



#endif /* INC_RTOS_INCS_H_ */
