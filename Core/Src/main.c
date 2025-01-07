/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "aht10.h"
#include "delay.h"
#include "rtthread.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
int fputc(int ch, FILE *f)
{
  while ((USART1->ISR & 0X40) == 0)
    ; // ѭ������,ֱ���������??

  USART1->TDR = (uint8_t)ch;
  return ch;
}
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
// 创建线程
static rt_thread_t thread1 = RT_NULL;

// 线程1入口函数
static void key_entry(void *parameter)
{
  while (1)
  {
    printf("thread1\n");
    rt_thread_delay(1000);
  }
}
// 创建线程

static int ThreadInit(void)
{
  rt_thread_t thread = rt_thread_create(
      "key_thread",
      key_entry,
      RT_NULL,
      4096 * 2,
      24,
      5);
  if (thread != RT_NULL)
  {
    rt_kprintf("create key_thread OK\n");
    rt_thread_startup(thread);
  }
  else
  {
    rt_kprintf("create key_thread failed\n");
    return -RT_ERROR;
  }
}
INIT_APP_EXPORT(ThreadInit);

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
void IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOC_CLK_ENABLE(); 
    __HAL_RCC_GPIOD_CLK_ENABLE(); 

    /*
        SCL - PD6		SDA-PC1
    */
    GPIO_Initure.Pin = GPIO_PIN_1;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_OD; 
    GPIO_Initure.Pull = GPIO_PULLUP;         
    GPIO_Initure.Speed = GPIO_SPEED_FAST;    
    HAL_GPIO_Init(GPIOC, &GPIO_Initure);

    GPIO_Initure.Pin = GPIO_PIN_6;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_OD; 
    GPIO_Initure.Pull = GPIO_PULLUP;         
    GPIO_Initure.Speed = GPIO_SPEED_FAST;    
    HAL_GPIO_Init(GPIOD, &GPIO_Initure);

    // IIC_SDA(1);
    // IIC_SCL(1);
}
void sda_in(void)
{
  GPIOC->MODER &= ~(3 << (1 * 2));
  GPIOC->MODER |= 0 << (1 * 2);
}

/* Configure SDA pin as output */
 void sda_out(void)
{
  GPIOC->MODER &= ~(3 << (1 * 2));
  GPIOC->MODER |= 1 << (1 * 2);
}
void IIC_SCL(uint8_t n)
{
  if (n)
  {
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, GPIO_PIN_SET);
  }
  else
  {
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, GPIO_PIN_RESET);
  }
}

void IIC_SDA(uint8_t n)
{
  if (n)
  {
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
  }
  else
  {
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
  }
}
/* Read SDA pin state */
uint8_t READ_SDA(void)
{
  return HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1);
}
i2c_driver_interface_t i2c_ops = {
    .pf_i2c_init = IIC_Init,
    .pf_i2c_sda_in = sda_in,
    .pf_i2c_sda_out = sda_out,
    .pf_set_scl = IIC_SCL,
    .pf_set_sda = IIC_SDA,
    .pf_get_sda = READ_SDA,
};
delay_interface_t delay_interface = {
    .pf_delay_us = delay_us,
};

//I2C_Driver aht10_i2c_ops = {
//   .Write =IIC_Write_Data,
//   .Read   = IIC_Read_Data
//};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
float temperature = 1, humidity = 1;
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
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  delay_init(80);
 // AHT10_Init();
  /* USER CODE END 2 */
  i2c_driver_t i2c_driver;
  iic_driver_inst(&i2c_driver, &i2c_ops, &delay_interface);
  AHT10_Init(&i2c_driver);                   
   


  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    temperature = AHT10_Read_Temperature(&i2c_driver);
    humidity = AHT10_Read_Humidity(&i2c_driver);
    printf("temperature:%.2f humidity:%.2f\n", temperature, humidity);
    rt_thread_delay(1000);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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
