/*
 * warship_task.c
 *
 *  Created on: 30 Mar 2023
 *      Author: wx
 */

#include "rtos_incs.h"
#include "warship_task.h"

volatile warship_state_e g_warship_state = 0;
volatile uint8_t charging_field = 0;
volatile time_t pbTick = 0;
volatile time_t battleTick = 0;


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if (GPIO_Pin == GPIO_PIN_13){
		//check debouncing
		if (HAL_GetTick()- pbTick > 10){
			//button press is a different press;
			switch(g_warship_state){
			case RESCUE:
				//check for double press only
				if (HAL_GetTick() - pbTick < 500){
					g_warship_state = BATTLE;
				}
				break;


			case BATTLE:
				battleTick = HAL_GetTick();
				g_warship_state = BATTLE_WARNING;
				break;


			case BATTLE_WARNING:
				//check if 500ms has passed since changing to warning mode
				if (HAL_GetTick() - battleTick < 500){
					//double press still considered under battle mode
					g_warship_state = RESCUE;
				} else {
					if (HAL_GetTick() - pbTick < 500){
					//single press in battle warning mode
					charging_field = 1;
					}
				}
				break;

			default:
				break;

			}
		}
		//reset tick counter for next press
		pbTick = HAL_GetTick();

	}
}

void warship_task(void* parameter){
	//init
	while(1){
		//loop
		switch(g_warship_state){
			case RESCUE:
				break;


			case BATTLE:
				break;


			case BATTLE_WARNING:
				break;

			default:
				break;

			}
		vTaskDelay(10);
	}
}

