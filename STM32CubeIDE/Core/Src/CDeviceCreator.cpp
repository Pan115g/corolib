/*
 * CDeviceCreator.cpp
 *
 *  Created on: May 9, 2025
 *      Author: pan.li
 */

#include "CDeviceCreator.h"
#include "CIrqCallback.h"

extern DMA_HandleTypeDef hdma_adc1;

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
    mAdc1.initiateChannels(adcChannels, adcNumOfChannels);

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


