/*
 * accel_rtos_task.c
 *
 *  Created on: Mar 7, 2023
 *      Author: wx
 */

#include "../../Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_magneto.h"
#include "stdio.h"
#include "../../tasks/inc/lis_task.h"



void lis_init(){

}

void lis_loop(){

}

void lis_task(void* argument){
	lis_init();
	while(1){
		lis_loop();
	}
}


