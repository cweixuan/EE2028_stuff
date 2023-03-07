/*
 * hts_task.c
 *
 *  Created on: Mar 7, 2023
 *      Author: wx
 */




#include "stm32l475e_iot01_tsensor.h"
#include "stdio.h"
#include "hts_task.h"

void hts_init(){

}

void hts_loop(){

}

void hts_task(void* argument){
	hts_init();
	while(1){
		hts_loop();
	}
}


