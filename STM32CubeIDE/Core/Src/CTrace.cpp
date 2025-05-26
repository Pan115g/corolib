/*
 * CTrace.cpp
 *
 *  Created on: May 22, 2025
 *      Author: pan.li
 */

#include "CTrace.h"
#include <cstdio>
#include <cstring>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"
#include "task.h"

#ifdef __cplusplus
extern "C" {
#endif

TIM_HandleTypeDef htim10;
volatile uint32_t freertosStatsCounter;

void Error_Handler();


void MX_TIM10_Init(void)
{
  htim10.Instance = TIM10;
  htim10.Init.Prescaler = 0;
  htim10.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim10.Init.Period = 840;
  htim10.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim10.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim10) != HAL_OK)
  {
    Error_Handler();
  }
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
  if(htim_base->Instance==TIM10)
  {
    __HAL_RCC_TIM10_CLK_ENABLE();

    HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
  }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{
  if(htim_base->Instance==TIM10)
  {
    __HAL_RCC_TIM10_CLK_DISABLE();

    HAL_NVIC_DisableIRQ(TIM1_UP_TIM10_IRQn);
  }
}

void TIM1_UP_TIM10_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim10);
  freertosStatsCounter += 1;
}



void configureTimerForRunTimeStats(void)
{
    freertosStatsCounter = 0;
    HAL_TIM_Base_Start_IT(&htim10);
}

unsigned long getRunTimeCounterValue(void)
{
    return freertosStatsCounter;
}

#ifdef __cplusplus
}
#endif

#ifdef DEBUG

CTrace::CTrace()
{
    const osThreadAttr_t defaultTask_attributes = {
      .name = "traceTask",
      .stack_size = 128 * 16,
      .priority = (osPriority_t) osPriorityLow,
    };
    mThread = osThreadNew(CTrace::runThread, NULL, &defaultTask_attributes);
}

void CTrace::runThread(void*)
{
    TaskStatus_t *pxTaskStatusArray = NULL;
    uint32_t uxArraySize, x;
    uint32_t ulTotalRunTime;
    float runtimePercentage;

    while (1)
    {
        osDelay(3000);
        uint32_t xFreeBytesRemaining = xPortGetFreeHeapSize();
        printf("Free heap = %lu\n", xFreeBytesRemaining);

        uxArraySize = uxTaskGetNumberOfTasks();
        if (pxTaskStatusArray == NULL)
        {
            pxTaskStatusArray = (TaskStatus_t*)pvPortMalloc(uxArraySize * sizeof(TaskStatus_t));
        }

        std::memset(pxTaskStatusArray, 0, uxArraySize * sizeof(TaskStatus_t));
        if (pxTaskStatusArray)
        {
            uxArraySize = uxTaskGetSystemState(pxTaskStatusArray, uxArraySize, &ulTotalRunTime);
            printf("Task count = %lu\n", uxArraySize);
            printf("id           name  priority    state    cpu    stack\n");
            for (x = 0; x < uxArraySize; x++)
            {
                runtimePercentage = (float)pxTaskStatusArray[x].ulRunTimeCounter / (float)ulTotalRunTime * 100.0F;
                printf("%2ld%15s%10ld%9d%7.1f%9d\n", x, pxTaskStatusArray[x].pcTaskName,
                        pxTaskStatusArray[x].uxCurrentPriority, pxTaskStatusArray[x].eCurrentState,
                        runtimePercentage, pxTaskStatusArray[x].usStackHighWaterMark);
            }
        }
    }
}

#endif
