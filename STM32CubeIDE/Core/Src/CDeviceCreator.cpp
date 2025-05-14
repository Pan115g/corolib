/*
 * CDeviceCreator.cpp
 *
 *  Created on: May 9, 2025
 *      Author: pan.li
 */

#include "CDeviceCreator.h"
#include "CIrqCallback.h"

CDeviceCreator& CDeviceCreator::getInstance()
{
    static CDeviceCreator creator;
    return creator;
}

CDeviceCreator::CDeviceCreator() : mUart2(USART2, 115200, '\r')
{
    CIrqCallback::getInstance().registerUart2IRQHandler([&]() {
        mUart2.irqHandler();
    });
    CIrqCallback::getInstance().registerUart2RxCallback([this](){
        mUart2.dataReceived();
    });
    mUart2.registerMessageReceived([this](const uint8_t* data, const uint32_t size){
        mUart2.write({(uint8_t*)data, size});
      });
}


