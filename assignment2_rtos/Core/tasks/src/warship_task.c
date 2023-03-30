/*
 * warship_task.c
 *
 *  Created on: 30 Mar 2023
 *      Author: wx
 */

#include "rtos_incs.h"
#include "warship_task.h"

warship_state_e warship_state = 0;

void warship_task(void* parameter){
	//init
	while(1){
		//loop
		switch(warship_state){
		case SENTRY:
			break;
		case WARNING:
			break;
		case BATTLE:
			break;
		}

		vTaskDelay(10);
	}
}

