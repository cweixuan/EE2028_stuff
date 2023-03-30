/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct {
	float x;
	float y;
	float z;
}axis_data_t;

typedef struct{
	float pressure;
}lps_data_t;

typedef struct{
	float temperature;
	float humidity;
}hts_data_t;


typedef enum {
	SENTRY=0,
	WARNING=1,
	BATTLE=2
}warship_state_e;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){

}
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void uart_send(warship_state_e warship_state, axis_data_t gyro_data, axis_data_t accel_data, axis_data_t mag_data,
		lps_data_t pressure_data, hts_data_t temphum_data){

	static char tx_buffer[256];
	static uint8_t tx_len = 0;
	switch(warship_state){
		case SENTRY:
			tx_len = snprintf(tx_buffer, 256, 			"G: X:%.4fdps | Y:%.4fdps | Z:%.4fdps \r\n",
								gyro_data.x, gyro_data.y, gyro_data.z);
			tx_len += snprintf(tx_buffer+tx_len, 256,	"A: X:%.4fg   | Y:%.4fg   | Z:%.4fg \r\n",
								accel_data.x, accel_data.y, accel_data.z);
			tx_len += snprintf(tx_buffer+tx_len, 256, 	"P: %.4fpa    | H:%.4f	  | T: %.4f \r\n\n",
								pressure_data.pressure, temphum_data.humidity, temphum_data.temperature);
			break;
		case WARNING:
			tx_len = snprintf(tx_buffer, 256, 			"G: X:%.4fdps | Y:%.4fdps | Z:%.4fdps \r\n",
								gyro_data.x, gyro_data.y, gyro_data.z);
			tx_len += snprintf(tx_buffer+tx_len, 256,	"A: X:%.4fg   | Y:%.4fg   | Z:%.4fg \r\n",
								accel_data.x, accel_data.y, accel_data.z);
			tx_len += snprintf(tx_buffer+tx_len, 256, 	"P: %.4fpa    | H:%.4f	  | T: %.4f \r\n\n",
								pressure_data.pressure, temphum_data.humidity, temphum_data.temperature);
			break;
		case BATTLE:
			tx_len = snprintf(tx_buffer, 256, 			"G: X:%.4fdps | Y:%.4fdps | Z:%.4fdps \r\n",
								gyro_data.x, gyro_data.y, gyro_data.z);
			tx_len += snprintf(tx_buffer+tx_len, 256,	"A: X:%.4fg   | Y:%.4fg   | Z:%.4fg \r\n",
								accel_data.x, accel_data.y, accel_data.z);
			tx_len += snprintf(tx_buffer+tx_len, 256, 	"P: %.4fpa    | H:%.4f	  | T: %.4f \r\n\n",
								pressure_data.pressure, temphum_data.humidity, temphum_data.temperature);
			break;
		default:
			break;
	}
	HAL_UART_Transmit_DMA(&huart1, (uint8_t*)tx_buffer,tx_len);
}

axis_data_t get_gyro_data(){
	//get magnetometer data here and convert to readable and 3 axis
	axis_data_t gyro_data;
	gyro_data.x = 0.5;
	gyro_data.y = 0.5;
	gyro_data.z = 0.5;

	return gyro_data;
}

axis_data_t get_accel_data(){
	//get accelerometer data here and convert to readable and 3 axis
	axis_data_t accel_data;
	accel_data.x = 1;
	accel_data.y = 1;
	accel_data.z = 1;

	return accel_data;
}

axis_data_t get_mag_data(){
	//get magnetometer data here and convert to readable and 3 axis
	axis_data_t mag_data;
	mag_data.x = 5;
	mag_data.y = 10;
	mag_data.z = 0;

	return mag_data;
}

float get_pres_data(){
	//get pressure data here and convert to readable
	float pressure;
	pressure = 1.0;

	return pressure;
}

float get_temp_data(){
	//get temperature data here and convert to readable
	float temperature;
	temperature = 29.5;

	return temperature;
}

float get_humidity_data(){
	//get temperature data here and convert to readable
	float humidity;
	humidity = 50;

	return humidity;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  //variables
  time_t uart_tick = 0;
  time_t accel_tick = 0;
  time_t gyro_tick = 0;
  time_t lis_tick = 0;
  time_t temp_tick = 0;
  time_t humi_tick = 0;
  time_t pres_tick = 0;

  axis_data_t accel_data;
  axis_data_t gyro_data;
  axis_data_t mag_data;
  hts_data_t temphum_data;
  lps_data_t pressure_data;
  warship_state_e warship_state = SENTRY;


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  //todo: insert initialisation code here for all the peripherals


  while (1)
  {
	  if (uwTick - uart_tick > 1000){
		  uart_tick = uwTick;
		  uart_send(warship_state, gyro_data, accel_data, mag_data,pressure_data, temphum_data);
	  }
	  if (uwTick - lis_tick > 1000){
		  lis_tick = uwTick;
		  mag_data = get_mag_data();
	  }
	  if (uwTick - accel_tick > 1000){
		  accel_tick = uwTick;
		  accel_data = get_accel_data();
	  }
	  if (uwTick - gyro_tick > 1000){
		  gyro_tick = uwTick;
		  gyro_data = get_gyro_data();
	  }
	  if (uwTick - temp_tick > 1000){
		  temp_tick = uwTick;
		  temphum_data.temperature = get_temp_data();
	  }
	  if (uwTick - humi_tick > 1000){
		  humi_tick = uwTick;
		  temphum_data.humidity = get_humidity_data();
	  }
	  if (uwTick - pres_tick > 1000){
		  pres_tick = uwTick;
		  pressure_data.pressure = get_pres_data();
	  }



    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}



/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
