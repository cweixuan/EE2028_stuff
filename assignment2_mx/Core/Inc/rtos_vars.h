/*
 * rtos_vars.h
 *
 *  Created on: Mar 7, 2023
 *      Author: wx
 */

#ifndef INC_RTOS_VARS_H_
#define INC_RTOS_VARS_H_

#include "main.h"
#include "cmsis_os.h"
extern TaskHandle_t uart1_task_handle;
extern TaskHandle_t lis_task_handle;
extern TaskHandle_t lps_task_handle;
extern TaskHandle_t lsm_task_handle;
extern TaskHandle_t hts_task_handle;

extern QueueHandle_t UART1_queue;

#endif /* INC_RTOS_VARS_H_ */
