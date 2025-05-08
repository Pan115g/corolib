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

class CUartAdapter
{
    constexpr static uint32_t cBufferSize = static_cast<uint32_t>(1UL) << 7UL;
    constexpr static uint32_t cBufferMask = cBufferSize - 1UL;
public:
    CUartAdapter(void* handle, uint32_t baud);

    bool isInitialized(){return mInitialized;};

    void registerMessageReceived(std::function<void(const uint8_t*, const uint32_t)> messageReceived);

    void read();

    void write(const uint8_t *data, const uint32_t size);
private:
    void irqHandler();
    void dataReceived();
    UART_HandleTypeDef mUart;
    bool mInitialized {false};
    uint8_t mBuffer[cBufferSize];
    uint32_t mBufferIndex{0};
    std::function<void(const uint8_t*, const uint32_t)> mMessageReceived;
};


#endif /* INC_CUARTADAPTER_H_ */
