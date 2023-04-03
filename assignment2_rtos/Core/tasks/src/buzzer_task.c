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


extern warship_state_e g_warship_state;
extern uint8_t enable_extras;

#define RESCUE_BEEP_DUR 100
#define BATTLE_BEEP_DUR 100
#define BUZZER_VOLUME 70
#define ALARM_SPEED 1.01;

//Never gonna give you up~
//https://musescore.com/chlorondria/never-gonna-give-you-up
#define TEMPO	120
const uint16_t melody[] = {
REST, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_F4, NOTE_G4, NOTE_E4,
NOTE_D4, NOTE_C4, REST,
REST, NOTE_D4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_D4, NOTE_C4,
NOTE_C5, REST, NOTE_C5, NOTE_G4, REST,
NOTE_D4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_D4, NOTE_F4, NOTE_G4, REST,
REST, NOTE_E4, NOTE_D4, NOTE_C4, REST,
REST, NOTE_D4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_D4, NOTE_C4,
NOTE_G4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_G4, REST,
NOTE_F4, NOTE_G4, NOTE_A4, NOTE_F4,
NOTE_G4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_G4, NOTE_C4,
REST, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_D4,
REST, NOTE_G4, NOTE_A4, NOTE_G4, NOTE_C4, NOTE_D4, NOTE_F4, NOTE_D4,
NOTE_A4, NOTE_A4, NOTE_G4, NOTE_C4, NOTE_D4, NOTE_F4, NOTE_D4,
NOTE_G4, NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_D4, NOTE_D4,
NOTE_F4, NOTE_G4, NOTE_E4, NOTE_D4, NOTE_C4, NOTE_C4, NOTE_C4,
NOTE_G4, NOTE_F4, NOTE_C4, NOTE_D4, NOTE_F4, NOTE_D4,
NOTE_A4, NOTE_A4, NOTE_G4, NOTE_C4, NOTE_D4, NOTE_F4, NOTE_D4,
NOTE_C5, NOTE_E4, NOTE_F4, NOTE_D4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_F4, NOTE_D4,
NOTE_F4, NOTE_G4, NOTE_E4, NOTE_D4, NOTE_C4, NOTE_C4,
NOTE_G4, NOTE_F4, REST, };

const float note_length[] = { 8, 2, 2, 2, 2, 2, 3, 1, 10, 4, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 6, 4, 2,
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 4, 2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 4, 4, 10, 2, 2, 2, 2,
		2, 2, 2, 4, 4, 8, 2, 2, 2, 2, 2, 2, 2, 6, 1, 1, 1, 1, 3, 3, 6, 1, 1, 1, 1, 3, 3, 3, 1, 2, 1,
		1, 1, 1, 4, 2, 3, 1, 2, 2, 2, 4, 6, 1, 1, 1, 1, 3, 3, 6, 1, 1, 1, 1, 4, 2, 3, 1, 2, 1, 1, 1,
		1, 4, 2, 3, 1, 2, 2, 4, 8, 2,

};


void buzzer_set_freq(uint16_t freq){

	  __HAL_TIM_SET_PRESCALER(&htim3, 160000/freq);
}

void buzzer_task(void* pvParameters){

	  htim3.Instance->ARR = 100;
	  htim3.Instance->CCR3 = 0;
	  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
	  buzzer_set_freq(440);
	  uint16_t alarm = 440;
	  float dir = ALARM_SPEED;
	  time_t last_beep = 0;
	  float semiquaver_time = 60000 / (4 * TEMPO);
	  TickType_t last_wake_time;
	  uint16_t note_time;
	  uint8_t volume = 0;
	  while (1){
		  if (enable_extras == 1){
			  volume = BUZZER_VOLUME;
		  } else {
			  volume = 0;
		  }
		  switch (g_warship_state){
		  case RESCUE:
			  if (uwTick - last_beep > 5000){
				  last_beep = uwTick;
				  htim3.Instance->CCR3 = volume;
			  } else if (uwTick-last_beep > RESCUE_BEEP_DUR){
				  htim3.Instance->CCR3 = 0;

			  }
			  break;
		  case BATTLE:
			  buzzer_set_freq(880);
			  if (uwTick - last_beep > 1000){
				  last_beep = uwTick;
				  htim3.Instance->CCR3 = volume;
			  } else if (uwTick-last_beep > BATTLE_BEEP_DUR){
				  htim3.Instance->CCR3 = 0;

			  }
			  break;
		  case BATTLE_WARNING:
			  htim3.Instance->CCR3 = volume;
			  alarm *= dir;
			  buzzer_set_freq(alarm);
			  if (alarm < 440)
				  dir = ALARM_SPEED
			  else if (alarm > 1600 ){
				  dir = 1/ALARM_SPEED;
			  }
		  break;

		  case DEAD:
			  //INSERT RICKROLL
			  htim3.Instance->CCR3 = volume;
			  for (uint16_t i = 0; i < sizeof(melody) / sizeof(uint16_t); i++) {
			  				last_wake_time = xTaskGetTickCount();
			  				buzzer_set_freq(melody[i]);
			  				note_time = (semiquaver_time * note_length[i]);
			  				vTaskDelayUntil(&last_wake_time, note_time * 0.97);
			  				buzzer_set_freq(0);
			  				vTaskDelayUntil(&last_wake_time, note_time * 0.03);

			  			}
			  vTaskSuspend(buzzer_task_handle);
			  break;
		  }
		  vTaskDelay(10);
	  }

}

