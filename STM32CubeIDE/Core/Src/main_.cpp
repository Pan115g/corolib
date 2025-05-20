/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32f4xx_it.h"
#include "cmsis_os.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <array>
#include "Awaitable.h"
#include "FireAndForget.h"
#include "CUartAdapter.h"
#include "CAdcAdapter.h"
#include "CIrqCallback.h"
#include "CUartReadTask.h"
#include "CAdcReceiveTask.h"
#include "CTaskScheduler.h"
#include "CDeviceCreator.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
/* USER CODE BEGIN PFP */


/* USER CODE END PFP */
DMA_HandleTypeDef hdma_adc1;
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};


#define TRUE 1
#define FALSE 0
std::array<uint8_t, 10> buffer;
uint8_t  data_buffer[100];
uint8_t  recvd_data;
uint32_t count=0;
uint8_t  reception_complete = FALSE;


void testReceive(corolib::CUartAdapter& uart)
{
//    HAL_UART_Receive_IT(&uart.mUart, &recvd_data, 1UL);
    uart.read(buffer);
}

corolib::Awaitable<> writeUart(corolib::CTaskScheduler &scheduler, corolib::CUartAdapter& uart, const std::span<uint8_t> buffer)
{
    std::array<uint8_t, 128> buffer_loc;
    uint32_t data_size = buffer.size();
    std::ranges::copy(buffer.begin(), buffer.end(), buffer_loc.begin());
    co_await scheduler.schedule();
    HAL_UART_Transmit(&uart.mUart,(uint8_t*)buffer_loc.data(), data_size, HAL_MAX_DELAY);
}

corolib::Awaitable<> readUart(corolib::CTaskScheduler &scheduler, corolib::CUartAdapter& uart)
{
    std::array<uint8_t, 128> buffer_loc;
    char user_data[] = "Testing coroutines\r";
    HAL_UART_Transmit(&uart.mUart,(uint8_t*)user_data,strlen(user_data),HAL_MAX_DELAY);
    while(true)
    {
        uint32_t numOfBytes = co_await corolib::CUartReadTask(uart, buffer_loc);
        co_await writeUart(scheduler, uart, {(uint8_t*)buffer_loc.data(), numOfBytes});
    }
}


corolib::Awaitable<> readAdc(corolib::CTaskScheduler &scheduler, corolib::CAdcAdapter& adc, corolib::CUartAdapter& uart)
{
    std::array<uint8_t, 32> buffer_loc;
    uint16_t temperature = 0;
    float temperatureSum = 0;
    char sendBuffer[32];
    while(true)
    {
        uint32_t numOfBytes = co_await corolib::CAdcReceiveTask(adc, buffer_loc);
        for (int i = 0; i < 16; ++i)
        {
            temperature = buffer_loc[2*i] | buffer_loc[2*i+1] << 8;
            temperatureSum += (((float)temperature * 3.3/4096.0) - 0.76)/2.5 + 25.0;

        }
        temperatureSum /= 16.0;
        int res = std::snprintf(sendBuffer, sizeof(sendBuffer), "Cpu temperature: %3d\r", (int32_t)temperatureSum);
        co_await writeUart(scheduler, uart, {(uint8_t*)sendBuffer, 21UL});
        osDelay(500);
    }
}

void StartDefaultTask(void *argument)
{
    corolib::CTaskScheduler* scheduler = static_cast<corolib::CTaskScheduler*>(argument);
    corolib::fireAndForget(readUart(*scheduler, CDeviceCreator::getInstance().getUart2()));
    corolib::fireAndForget(readAdc(*scheduler, CDeviceCreator::getInstance().getAdc1(), CDeviceCreator::getInstance().getUart2()));

    //Task shall not exit
    while(1)
    {
        osDelay(10000);
    }
}

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

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();

  MX_DMA_Init();

  osKernelInitialize();

  static corolib::CTaskScheduler scheduler;
  /*static TestAtomic test_ato;
  uint32_t head = test_ato.mQueueHead.load(std::memory_order_acquire);
  uint32_t tail = test_ato.mQueueTail.load(std::memory_order_acquire);*/
  defaultTaskHandle = osThreadNew(StartDefaultTask, static_cast<void*>(&scheduler), &defaultTask_attributes);
  //testReceive(CDeviceCreator::getInstance().getUart2());
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  osKernelStart();

  //if we come here, we have enough heap.
  while (1)
  {
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
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 16;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ = 4;
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

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */


static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

}
/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM5 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM5) {
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
