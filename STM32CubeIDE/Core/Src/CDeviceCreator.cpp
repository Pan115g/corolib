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
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
  /* DMA1_Stream4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
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
DMA_HandleTypeDef hdma_spi2_rx;
DMA_HandleTypeDef hdma_spi2_tx;

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

void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hspi->Instance==SPI2)
  {
    __HAL_RCC_SPI2_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    hdma_spi2_rx.Instance = DMA1_Stream3;
    hdma_spi2_rx.Init.Channel = DMA_CHANNEL_0;
    hdma_spi2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_spi2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_spi2_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_spi2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_spi2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_spi2_rx.Init.Mode = DMA_NORMAL;
    hdma_spi2_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_spi2_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_spi2_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hspi,hdmarx,hdma_spi2_rx);

    hdma_spi2_tx.Instance = DMA1_Stream4;
    hdma_spi2_tx.Init.Channel = DMA_CHANNEL_0;
    hdma_spi2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_spi2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_spi2_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_spi2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_spi2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_spi2_tx.Init.Mode = DMA_NORMAL;
    hdma_spi2_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_spi2_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_spi2_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hspi,hdmatx,hdma_spi2_tx);

  }

}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
  if(hspi->Instance==SPI2)
  {
    __HAL_RCC_SPI2_CLK_DISABLE();

    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_2|GPIO_PIN_3);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_12);

    HAL_DMA_DeInit(hspi->hdmarx);
    HAL_DMA_DeInit(hspi->hdmatx);
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

void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(hrtc->Instance==RTC)
  {
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_RCC_RTC_ENABLE();

  }

}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* hrtc)
{
  if(hrtc->Instance==RTC)
  {
    __HAL_RCC_RTC_DISABLE();
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

CDeviceCreator::CDeviceCreator() : mUart2(USART2, 115200, '\r'), mAdc1{ADC1}, mSpi2Master{SPI2},
        mRtc{RTC}
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

    CIrqCallback::getInstance().registerDMA1Stream3IRQHandler([]() {
        HAL_DMA_IRQHandler(&hdma_spi2_rx);
    });

    CIrqCallback::getInstance().registerDMA1Stream4IRQHandler([]() {
        HAL_DMA_IRQHandler(&hdma_spi2_tx);
    });

    mRtc.setDateTime(25, RTC_MONTH_JUNE, 9, RTC_WEEKDAY_MONDAY, 17, 11, 59);
}


