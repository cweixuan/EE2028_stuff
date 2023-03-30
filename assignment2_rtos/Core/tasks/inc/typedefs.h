/*
 * typedefs.h
 *
 *  Created on: Mar 14, 2023
 *      Author: wx
 */

#ifndef TASKS_INC_TYPEDEFS_H_
#define TASKS_INC_TYPEDEFS_H_

typedef enum {
	RX = 0,
	TX = 1
}q_UART_type_t;

typedef struct {
	q_UART_type_t type;
	uint16_t char_len;
	char uart_chars[256];
}queue_UART_msg_t;




#endif /* TASKS_INC_TYPEDEFS_H_ */
