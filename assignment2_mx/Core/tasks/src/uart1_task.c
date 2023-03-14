/*
 * uart_sending_task.c
 *
 *  Created on: Mar 7, 2023
 *      Author: wx
 */

#include "rtos_incs.h"
#include "usart.h"
#include "uart1_task.h"




void uart1_task(void* pvParameters){
	//init code
	queue_UART_msg_t *uart_q;
	while(1){
		xQueueReceive(UART1_queue, &uart_q, 0);
		if (uart_q->type == TX){
			HAL_UART_Transmit_DMA(&huart1, (uint8_t*)uart_q->uart_chars, uart_q->char_len);
		}
		//running code
	}
}
