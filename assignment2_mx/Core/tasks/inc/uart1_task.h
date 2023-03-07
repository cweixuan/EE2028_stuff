/*
 * uart_sending_task.h
 *
 *  Created on: Mar 7, 2023
 *      Author: wx
 */

#ifndef TASKS_INC_UART1_TASK_H_
#define TASKS_INC_UART1_TASK_H_
#include "stdio.h"

typedef struct {
	uint16_t tx_len;
	char uart_chars[256];
}queue_UART_msg_t;

void uart1_init();
void uart1_loop();
void uart1_task(void* argument);


#endif /* TASKS_INC_UART1_TASK_H_ */
