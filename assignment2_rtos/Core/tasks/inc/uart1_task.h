/*
 * uart_sending_task.h
 *
 *  Created on: Mar 7, 2023
 *      Author: wx
 */

#ifndef TASKS_INC_UART1_TASK_H_
#define TASKS_INC_UART1_TASK_H_
#include "stdio.h"


void uart1_init();
void uart1_loop();
void uart1_task(void* pvParameters);


#endif /* TASKS_INC_UART1_TASK_H_ */
