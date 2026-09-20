/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "can.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "MPU6050.h"
#include "OLED.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DT 0.01f
#define GYRO_SCALE  131.0f
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t uart_rx_data=0x00;

uint8_t i2c_rx_data[2]={0};
int16_t origin;
volatile float angle_x=0.0f;
volatile float gyro_x_dps=0;
volatile float speed=0;

uint8_t can_tx_data[3];

volatile int32_t x_pos=64;
volatile int32_t y_pos=32;

volatile uint8_t can_tx_ready=0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  OLED_Init();
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN_Init();
  MX_USART1_UART_Init();
  MX_TIM1_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  MPU6050_Init();
  HAL_UART_Receive_IT(&huart1,&uart_rx_data,1);
  HAL_TIM_Base_Start_IT(&htim1);
  HAL_TIM_Base_Start_IT(&htim2);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	if(uart_rx_data!=0)
	{	
		OLED_ShowString(1,1,"angle:");
		OLED_ShowSignedNum(1,7,speed,5);
		OLED_ShowString(2,1,"uart_rx:");
		OLED_ShowChar(2,9,(char)uart_rx_data);
		OLED_ShowNum(3,1,x_pos,5);
		OLED_ShowNum(4,1,y_pos,5);
		
		if(can_tx_ready==1)
		{	
			CAN_TxHeaderTypeDef t={0};
			t.StdId=0X001;
			t.RTR=CAN_RTR_DATA;
			t.IDE=CAN_ID_STD;
			t.DLC=3;
			t.TransmitGlobalTime=DISABLE;
			uint32_t txmailbox;
			
			can_tx_data[0]=uart_rx_data;
			can_tx_data[1]=(uint8_t)x_pos;
			can_tx_data[2]=(uint8_t)y_pos;
			
			HAL_CAN_AddTxMessage(&hcan,&t,can_tx_data,&txmailbox);
			
			can_tx_ready=0;
		}
	}
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART1)
	{
		HAL_UART_Receive_IT(&huart1,&uart_rx_data,1);
	}
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM1)
	{
		HAL_I2C_Mem_Read(&hi2c1,MPU6050_ADDRESS,MPU6050_GYRO_XOUT_H,I2C_MEMADD_SIZE_8BIT,i2c_rx_data,2,100);
		origin  = (i2c_rx_data[0]<<8);
		origin |= (i2c_rx_data[1]);
		gyro_x_dps = origin / GYRO_SCALE;
		angle_x += gyro_x_dps * DT;
		speed = angle_x ;
		
	}
	if(htim->Instance == TIM2)
	{
		switch (uart_rx_data)
		{
			case('W'):
			{
				y_pos=y_pos+((int32_t)(speed*0.1f));
				if(y_pos>63)
				{
					y_pos=63;
				}
				if(y_pos<0)
				{
					y_pos=0;
				}
				break;
			}
			case('S'):
			{
				y_pos=y_pos-((int32_t)(speed*0.1f));
				if(y_pos>63)
				{
					y_pos=63;
				}
				if(y_pos<0)
				{
					y_pos=0;
				}
				break;
			}
			case('A'):
			{
				x_pos=x_pos-((int32_t)(speed*0.1f));
				if(x_pos>127)
				{
					x_pos=127;
				}
				if(x_pos<0)
				{
					x_pos=0;
				}
				break;
			}
			case('D'):
			{
				x_pos=x_pos+((int32_t)(speed*0.1f));
				if(x_pos>127)
				{
					x_pos=127;
				}
				if(x_pos<0)
				{
					x_pos=0;
				}
				break;
			}
			default:
			{
				break;
			}
		}
		if(uart_rx_data == 'W' || uart_rx_data == 'A' || uart_rx_data == 'S' || uart_rx_data == 'D')
		{
			can_tx_ready=1;
		}
	}
}
/* USER CODE END 4 */

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
	  for (volatile uint32_t i = 0; i < 1000000; i++);
	  HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_5);
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
