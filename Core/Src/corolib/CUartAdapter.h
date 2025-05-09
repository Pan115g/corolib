/*
 * Task.h
 *
 *  Created on: May 6, 2025
 *      Author: pan.li
 */

#ifndef INC_CUARTADAPTER_H_
#define INC_CUARTADAPTER_H_

#include "stm32f4xx_hal.h"
#include <functional>
#include <span>

namespace corolib
{
    class CUartAdapter
    {
    public:
        CUartAdapter(USART_TypeDef *handle, const uint32_t baud, const uint8_t delimiter);

        bool isInitialized(){return mInitialized;};

        void registerMessageReceived(std::function<void(const uint8_t*, const uint32_t)> messageReceived);

        bool read(std::span<uint8_t> buffer);

        void write(const std::span<uint8_t> data);

        void irqHandler();
        void dataReceived();

        UART_HandleTypeDef mUart;
        private:
        uint8_t mDelimiter;
        bool mInitialized {false};
        uint8_t* mBuffer;
        uint32_t mBufferIndex{0};
        std::function<void(const uint8_t*, const uint32_t)> mMessageReceived;
    };
}

#endif /* INC_CUARTADAPTER_H_ */
