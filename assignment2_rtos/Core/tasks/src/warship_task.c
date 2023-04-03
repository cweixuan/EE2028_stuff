/*
 * warship_task.c
 *
 *  Created on: 30 Mar 2023
 *      Author: wx
 */

#include "rtos_incs.h"
#include "tim.h"
#include "gpio.h"
#include "stm32l4xx.h"
#include "main.h"
#include "warship_task.h"

volatile warship_state_e g_warship_state = 0;
volatile uint8_t enable_extras = 0;

//global vars only for this file
volatile static time_t pbTick = 0;
volatile static time_t battleTick = 0;
volatile static time_t energyTick = 0;
volatile static uint8_t energy = 1;

void pb_func(){		//check debouncing
	if (HAL_GetTick()- pbTick > 50){
		//button press is a different press;
		switch(g_warship_state){
		case RESCUE:
			//check for double press only
			if (HAL_GetTick() - pbTick < 500){
				uart_notify_from_isr();
				g_warship_state = BATTLE;
				//toggle it again so the it undos the toggle from the first press of this doublepress
				enable_extras = 1-enable_extras;
			} else {
				enable_extras = 1-enable_extras;
			}
			break;


		case BATTLE:
			battleTick = HAL_GetTick();
			uart_notify_from_isr();
			g_warship_state = BATTLE_WARNING;
			break;


		case BATTLE_WARNING:
			//check if 500ms has passed since changing to warning mode
			if (HAL_GetTick() - battleTick < 500){
				//double press still considered under battle mode
				uart_notify_from_isr();
				g_warship_state = RESCUE;
			} else {
				if (HAL_GetTick() - energyTick > 500){
				energyTick = uwTick;
				//single press in battle warning mode
				energy += 2;
				energy = (energy < 11) ? energy : 11;
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


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if (GPIO_Pin == PUSHBUTTON_Pin){
		pb_func();
	}
	if (GPIO_Pin == LPS_DRDY_Pin){
		lps_notify_from_isr();
	}
	if (GPIO_Pin == LSM_DRDY_Pin){
		lsm_notify_from_isr();
	}
	if (GPIO_Pin == LIS_DRDY_Pin){
		lis_notify_from_isr();
	}
	if (GPIO_Pin == HTS_DRDY_Pin){
		hts_notify_from_isr();
	}
}

void warship_task(void* parameter){
	//green light, which isn't as fun :(
	htim15.Instance->ARR = 1330; //11 * 11 * 11
	htim15.Instance->CCR1 = 000;
	HAL_TIM_PWM_Start(&htim15, TIM_CHANNEL_1);
	//orange and blue light
	//default state is rescue, which has solid lights
	htim8.Instance->CCR4 = 10000;
	HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_4);
	//init
	while(1){
		//loop
		if (enable_extras){
			htim15.Instance->CCR1 = energy * energy * energy;
		} else {

			htim15.Instance->CCR1 = 0;
		}

		switch(g_warship_state){
			case RESCUE:
				//solid leds
				htim8.Instance->CCR4 = 5000;
				__HAL_TIM_SET_PRESCALER(&htim8, 32);
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

				if (HAL_GetTick() - battleTick >= 5000){
					//more than 5 seconds has passed
					if (energy >= 4){
						energy -=3;
						g_warship_state = BATTLE;
						uart_notify();
					} else {
						//gg no re
						g_warship_state = DEAD;
						uart_notify();
					}
				}
				break;
			case DEAD:
				//blink at 0
				htim8.Instance->CCR4 = 0;
				HAL_GPIO_DeInit(GPIOC, GPIO_PIN_9);
				if (enable_extras){
					uint16_t explosion_led = energy * energy * energy;
					while (explosion_led < 1330){
						htim15.Instance->CCR1 = explosion_led;
						explosion_led += 10;
						vTaskDelay(10);
					}
					vTaskDelay(500);
					HAL_GPIO_DeInit(GPIOB, GPIO_PIN_14);
				}
				vTaskSuspend(warship_task_handle);

				//insert an event flag here that only triggers when
				//everything else has shutdown lol
//				int* hardfaultme_pls;
//				*hardfaultme_pls = "hahah";
				break;

			default:
				break;

			}
		vTaskDelay(1);
	}
}

