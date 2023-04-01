/*
 * warship_task.c
 *
 *  Created on: 30 Mar 2023
 *      Author: wx
 */

#include "rtos_incs.h"
#include "tim.h"
#include "gpio.h"
#include "warship_task.h"

volatile warship_state_e g_warship_state = 0;

//global vars only for this file
volatile static time_t pbTick = 0;
volatile static time_t battleTick = 0;
volatile static uint8_t energy = 0;


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
					if (HAL_GetTick() - pbTick > 500){
					//single press in battle warning mode
					energy = (energy <= 10) ? energy+2 : 10;
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
	//green light, which isn't as fun :(
	//led set to solid
	htim15.Instance->CCR1 = 000;
	HAL_TIM_PWM_Start(&htim15, TIM_CHANNEL_1);
	//orange and blue light
	//default state is rescue, which has solid lights
	htim8.Instance->CCR4 = 10000;
	HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_4);
	//init
	while(1){
		//loop
		htim15.Instance->CCR1 = energy * energy * energy;
		switch(g_warship_state){
			case RESCUE:
				//solid leds
				htim8.Instance->CCR4 = 10000;
				__HAL_TIM_SET_PRESCALER(&htim8, 1599);
				break;


			case BATTLE:
				//blink at 1hz
				htim8.Instance->CCR4 = 4999;
				__HAL_TIM_SET_PRESCALER(&htim8, 1599);
				break;


			case BATTLE_WARNING:
				//blink at 3hz
				htim8.Instance->CCR4 = 4999;
				__HAL_TIM_SET_PRESCALER(&htim8, 532);

				if (uwTick - battleTick > 5000){
					//more than 5 seconds has passed
					if (energy >= 3){
						energy -=3;
						g_warship_state = BATTLE;
					} else {
						//gg no re
						g_warship_state = DEAD;
					}
				}
				break;
			case DEAD:
				//blink at 0
				HAL_GPIO_DeInit(GPIOC, GPIO_PIN_9);
				HAL_GPIO_DeInit(GPIOB, GPIO_PIN_14);
				//insert an event flag here that only triggers when
				//everything else has shutdown lol
//				int* hardfaultme_pls;
//				*hardfaultme_pls = "hahah";
				break;

			default:
				break;

			}
		vTaskDelay(10);
	}
}

