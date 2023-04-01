/*
 * buzzer_task.c
 *
 *  Created on: Mar 28, 2023
 *      Author: wx
 */
#include "rtos_incs.h"
#include "stm32l475xx.h"
#include "tim.h"
#include "stdio.h"
#include "buzzer_task.h"




void buzzer_task(void* pvParameters){

	  htim3.Instance->ARR = 100;
	  htim3.Instance->CCR3 = 2;
	  __HAL_TIM_SET_PRESCALER(&htim3, 400);
	  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
	  uint16_t presc = 400;
	  int dir = 1;


	  while (presc > 0){
		  presc += dir;
		  __HAL_TIM_SET_PRESCALER(&htim3, presc);
	  	  vTaskDelay(100);
		  if (presc < 100 || presc > 400 ){
			  dir = -dir;
		  }
	  }

}

