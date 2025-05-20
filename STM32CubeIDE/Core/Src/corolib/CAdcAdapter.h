/*
 * CAdcAdapter.h
 *
 *  Created on: May 19, 2025
 *      Author: pan.li
 */

#ifndef SRC_COROLIB_CADCADAPTER_H_
#define SRC_COROLIB_CADCADAPTER_H_

#include "stm32f4xx_hal.h"
#include <span>

namespace corolib {

    class CAdcAdapter {
    public:
        CAdcAdapter(ADC_TypeDef* adc);
        void initiateChannels(const uint32_t channelInputs[], const uint32_t numOfChannels);

        bool startConversion(std::span<uint8_t> buffer);

        uint32_t getNumberOfChannels() {return mNumOfChannel;};
    private:

        ADC_HandleTypeDef mAdc;
        bool mInitialized {false};
        uint32_t mNumOfChannel;
    };

} /* namespace corolib */

#endif /* SRC_COROLIB_CADCADAPTER_H_ */
