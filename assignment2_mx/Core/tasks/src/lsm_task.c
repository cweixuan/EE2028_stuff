/*
 * lsm_task.c
 *
 *  Created on: Mar 7, 2023
 *      Author: wx
 */

#include "../../Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_accelero.h"
#include "../../Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_gyro.h"
#include "stdio.h"
#include "../../tasks/inc/lsm_task.h"

void lsm_init(){

}

void lsm_loop(){

}

void lsm_task(void* argument){
	lsm_init();
	while(1){
		lsm_loop();
	}
}



