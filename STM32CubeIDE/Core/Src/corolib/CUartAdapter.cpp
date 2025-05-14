/*
 * Task.cpp
 *
 *  Created on: May 6, 2025
 *      Author: pan.li
 */

#include "CUartAdapter.h"
#include <string.h>

namespace corolib
{
    CUartAdapter::CUartAdapter(USART_TypeDef *handle, const uint32_t baud, const uint8_t delimiter)
    : mUart{handle}, mDelimiter{delimiter}, mMessageReceived{}
    {
        mUart.Instance = handle;
        mUart.Init.BaudRate = baud;
        mUart.Init.WordLength = UART_WORDLENGTH_8B;
        mUart.Init.StopBits = UART_STOPBITS_1;
        mUart.Init.Parity = UART_PARITY_NONE;
        mUart.Init.Mode = UART_MODE_TX_RX;
        mUart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
        mUart.Init.OverSampling = UART_OVERSAMPLING_16;

        mInitialized = HAL_UART_Init(&mUart) == HAL_OK;
    }

    bool CUartAdapter::read(std::span<uint8_t> buffer)
    {
        mBuffer = buffer.data();
        mBufferIndex = 0;
        return HAL_UART_Receive_IT(&mUart, &mBuffer[mBufferIndex], 1UL) != HAL_ERROR;
    }

    void CUartAdapter::write(const std::span<uint8_t> data)
    {
        HAL_UART_Transmit_IT(&mUart, data.data(), data.size());
    }

    void CUartAdapter::irqHandler()
    {
        HAL_UART_IRQHandler(&mUart);
    }

    void CUartAdapter::dataReceived()
    {
        if(mBuffer[mBufferIndex] == mDelimiter)
        {
            mMessageReceived(mBuffer, mBufferIndex + 1UL);
            mBufferIndex = 0;
        }
        else
        {
            mBufferIndex += 1UL;
            HAL_UART_Receive_IT(&mUart, &mBuffer[mBufferIndex], 1UL);
        }
    }

    void CUartAdapter::registerMessageReceived(std::function<void(const uint8_t*, const uint32_t)> messageReceived)
    {
        mMessageReceived = messageReceived;
    }
}


