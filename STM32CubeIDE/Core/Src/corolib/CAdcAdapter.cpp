/*
 * CAdcAdapter.cpp
 *
 *  Created on: May 19, 2025
 *      Author: pan.li
 */

#include <CAdcAdapter.h>

namespace corolib {

    CAdcAdapter::CAdcAdapter(ADC_TypeDef* hadc)
    : mNumOfChannel{0}{
        mAdc.Instance = hadc;
        mAdc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
        mAdc.Init.Resolution = ADC_RESOLUTION_12B;
        mAdc.Init.ScanConvMode = ENABLE;
        mAdc.Init.ContinuousConvMode = ENABLE;
        mAdc.Init.DiscontinuousConvMode = DISABLE;
        mAdc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
        mAdc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
        mAdc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
        mAdc.Init.NbrOfConversion = mNumOfChannel;
        mAdc.Init.DMAContinuousRequests = ENABLE;
        mAdc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
        mInitialized = (HAL_ADC_Init(&mAdc) == HAL_OK);
    }

    bool CAdcAdapter::startConversion(std::span<uint8_t> buffer)
    {
        return mInitialized && HAL_ADC_Start_DMA(&mAdc, (uint32_t*)buffer.data(), mNumOfChannel) != HAL_ERROR;
    }

    void CAdcAdapter::initiate(const uint32_t channelInputs[], const uint32_t numOfChannels)
    {
        mNumOfChannel = numOfChannels;
        ADC_ChannelConfTypeDef sConfig = {0};

        for (uint32_t i = 0; i < numOfChannels; ++i)
        {
            sConfig.Channel = channelInputs[i];
            sConfig.Rank = i + 1;
            sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
            if (HAL_ADC_ConfigChannel(&mAdc, &sConfig) != HAL_OK)
            {
                mInitialized = false;
                break;
            }
        }
    }

} /* namespace corolib */
