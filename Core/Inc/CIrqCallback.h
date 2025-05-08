/*
 * CIrqCallback.h
 *
 *  Created on: May 7, 2025
 *      Author: pan.li
 */

#ifndef INC_CIRQCALLBACK_H_
#define INC_CIRQCALLBACK_H_

#include "stm32f411xe.h"
#include <functional>

class CIrqCallback
{
public:
    void registerUart2IRQHandler(std::function<void()> c);
    void registerUart2RxCallback(std::function<void()> c);
    void registerUart2TxCallback(std::function<void()> c);
    void invokeIRQHandler(IRQn_Type irqN);
    void invokeRxCallback(IRQn_Type irqN);
    void invokeTxCallback(IRQn_Type irqN);
    static CIrqCallback& getInstance();

private:
    CIrqCallback();
    std::function<void()> mUart2IRQHandler;
    std::function<void()> mUart2RxCallback;
    std::function<void()> mUart2TxCallback;
};



#endif /* INC_CIRQCALLBACK_H_ */
