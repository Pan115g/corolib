/*
 * CDeviceCreator.cpp
 *
 *  Created on: May 9, 2025
 *      Author: pan.li
 */

#include "CDeviceCreator.h"
#include "CIrqCallback.h"


#ifdef __cplusplus
extern "C" {
#endif

#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB

void Error_Handler(void);

void MX_GPIO_Init(void)
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


void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

}

void HAL_MspInit(void)
{
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  HAL_NVIC_SetPriority(PendSV_IRQn, 15, 0);
}

DMA_HandleTypeDef hdma_adc1;

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
  if(hadc->Instance==ADC1)
  {

    __HAL_RCC_ADC1_CLK_ENABLE();


    hdma_adc1.Instance = DMA2_Stream0;
    hdma_adc1.Init.Channel = DMA_CHANNEL_0;
    hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc1.Init.Mode = DMA_NORMAL;
    hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;
    hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hadc,DMA_Handle, hdma_adc1);
  }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
  if(hadc->Instance==ADC1)
  {
    __HAL_RCC_ADC1_CLK_DISABLE();

    HAL_DMA_DeInit(hadc->DMA_Handle);
  }
}

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(huart->Instance==USART2)
  {

    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = USART_TX_Pin|USART_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


    HAL_NVIC_SetPriority(USART2_IRQn, 15, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
  if(huart->Instance==USART2)
  {
    __HAL_RCC_USART2_CLK_DISABLE();

    HAL_GPIO_DeInit(GPIOA, USART_TX_Pin|USART_RX_Pin);

    HAL_NVIC_DisableIRQ(USART2_IRQn);
  }
}

#ifdef __cplusplus
}
#endif

CDeviceCreator& CDeviceCreator::getInstance()
{
    static CDeviceCreator creator;
    return creator;
}

CDeviceCreator::CDeviceCreator() : mUart2(USART2, 115200, '\r'), mAdc1{ADC1}
{
    const uint32_t adcNumOfChannels = 16UL;
    uint32_t adcChannels[adcNumOfChannels] = {
            ADC_CHANNEL_TEMPSENSOR, ADC_CHANNEL_TEMPSENSOR, ADC_CHANNEL_TEMPSENSOR, ADC_CHANNEL_TEMPSENSOR,
            ADC_CHANNEL_TEMPSENSOR, ADC_CHANNEL_TEMPSENSOR, ADC_CHANNEL_TEMPSENSOR, ADC_CHANNEL_TEMPSENSOR,
            ADC_CHANNEL_TEMPSENSOR, ADC_CHANNEL_TEMPSENSOR, ADC_CHANNEL_TEMPSENSOR, ADC_CHANNEL_TEMPSENSOR,
            ADC_CHANNEL_TEMPSENSOR, ADC_CHANNEL_TEMPSENSOR, ADC_CHANNEL_TEMPSENSOR, ADC_CHANNEL_TEMPSENSOR
    };
    mAdc1.initiate(adcChannels, adcNumOfChannels);

    CIrqCallback::getInstance().registerUart2IRQHandler([this]() {
        mUart2.irqHandler();
    });
    CIrqCallback::getInstance().registerUart2RxCallback([this](){
        mUart2.dataReceived();
    });

    CIrqCallback::getInstance().registerDMA2Stream0IRQHandler([]() {
        HAL_DMA_IRQHandler(&hdma_adc1);
    });
}


