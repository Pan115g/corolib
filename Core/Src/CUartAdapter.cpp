/*
 * Task.cpp
 *
 *  Created on: May 6, 2025
 *      Author: pan.li
 */

#include "CUartAdapter.h"
#include "CIrqCallback.h"
#include <string.h>

uint8_t  recvd_data;

CUartAdapter::CUartAdapter(void* handle, uint32_t baud)
: mMessageReceived{}
{
    memset(&mUart, 0, sizeof(mUart));
    mUart.Instance = static_cast<USART_TypeDef*>(handle);
    mUart.Init.BaudRate = baud;
    mUart.Init.WordLength = UART_WORDLENGTH_8B;
    mUart.Init.StopBits = UART_STOPBITS_1;
    mUart.Init.Parity = UART_PARITY_NONE;
    mUart.Init.Mode = UART_MODE_TX_RX;
    mUart.Init.HwFlowCtl = UART_HWCONTROL_NONE;

    if ((mInitialized = HAL_UART_Init(&mUart)) == HAL_OK)
    {
        CIrqCallback::getInstance().registerUart2IRQHandler([this]() {
            irqHandler();
        });
        CIrqCallback::getInstance().registerUart2RxCallback([this](){
            dataReceived();
        });
    }
}

void CUartAdapter::read()
{
    HAL_UART_Receive_IT(&mUart, &recvd_data, 1UL);
}

void CUartAdapter::write(const uint8_t *data, const uint32_t size)
{
    HAL_UART_Transmit_IT(&mUart, data, size);
}

void CUartAdapter::irqHandler()
{
    HAL_UART_IRQHandler(&mUart);
}

void CUartAdapter::dataReceived()
{
//    uint8_t data = mBuffer[mBufferIndex];
    mBuffer[mBufferIndex] = recvd_data;
    mBufferIndex += 1UL;
    if(recvd_data == '\r')
    {
        mMessageReceived(mBuffer, mBufferIndex);
        mBufferIndex = 0;
    }
}

void CUartAdapter::registerMessageReceived(std::function<void(const uint8_t*, const uint32_t)> messageReceived)
{
    mMessageReceived = messageReceived;
}



