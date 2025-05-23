/*
 * CTrace.cpp
 *
 *  Created on: May 22, 2025
 *      Author: pan.li
 */

#include "CTrace.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"

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
  freertosStatsCounter++;
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

