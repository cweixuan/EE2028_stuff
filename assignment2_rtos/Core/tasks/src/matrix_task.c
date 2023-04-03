/*
 * matrix_task.c
 *
 *  Created on: Apr 3, 2023
 *      Author: cwx
 */


#include "rtos_incs.h"
#include "matrix_task.h"
#include "i2c.h"
#include "HT16K33.h"

extern warship_state_e g_warship_state;
extern uint8_t enable_extras;

enum blink_type_t {
    BLINK_OFF = 0,
    BLINK_2HZ,
    BLINK_1HZ,
};

void matrix_init() {
	//init
	uint8_t tmp;
//	HAL_I2C_Mem_Read(&hi2c1, 0x70, 0x20, I2C_MEMADD_SIZE_8BIT, &tmp, 1, 0xFF);
	tmp = 0x21;
	HAL_I2C_Master_Transmit(&hi2c1, 0x70, &tmp, 1, 0xFF);
//	HAL_I2C_Mem_Read(&hi2c1, 0x70, 0xEF, I2C_MEMADD_SIZE_8BIT, &tmp, 1, 0xFF);
	//set brightness
	uint8_t brightness = 4;
	tmp =(0xE0 | brightness);
	HAL_I2C_Master_Transmit(&hi2c1, 0x70, &tmp, 1, 0xFF);
//	HAL_I2C_Mem_Write(&hi2c1, 0x70, (0xE0 | brightness), I2C_MEMADD_SIZE_8BIT, (uint8_t*)NULL, 0,0xFFFF);
	//set blink mode
	tmp = (0x80 | 0x01 | (BLINK_OFF << 1));
	HAL_I2C_Master_Transmit(&hi2c1, 0x70, &tmp, 1, 0xFF);
//	HAL_I2C_Mem_Write(&hi2c1, 0x70, (0x80 | 0x01 | (BLINK_OFF << 1)), I2C_MEMADD_SIZE_8BIT, (uint8_t*)NULL, 0,0xFFFF);
}


void matrix_task(void *argument) {
//	TickType_t last_wake_time;
//	matrix_init();
//	uint8_t tmp[16];
//	tmp[0] = 0xFF;
//	tmp[1] = 0x99;
//	memset(tmp+2,0,14);
//	HAL_I2C_Mem_Write(&hi2c1, 0x70, 0x00, I2C_MEMADD_SIZE_8BIT, tmp, 16,0xFFFF);
	//DOESN'T WORK, matrix requires 5v logic
	while (1) {
		vTaskDelay(100);
	}
}




