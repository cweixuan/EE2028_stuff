/*
 * uart_sending_task.c
 *
 *  Created on: Mar 7, 2023
 *      Author: wx
 */

#include "rtos_vars.h"
#include "uart1_task.h"


void uart1_task(void* argument){
	//init code
	static queue_UART_msg_t uart_q;
	while(1){
		xQueueReceive(UART1_queue, &uart_q, 100000);
		//running code
	}
}
