/*
 * CIrqCallback.h
 *
 *  Created on: May 7, 2025
 *      Author: pan.li
 */

#ifndef INC_CIRQCALLBACK_H_
#define INC_CIRQCALLBACK_H_

#include "stm32f411xe.h"
#include "stm32f4xx_hal.h"
#include <functional>

#ifdef __cplusplus
extern "C" {
#endif
void USART2_IRQHandler(void);
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
//void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);

#ifdef __cplusplus
}
#endif

class CIrqCallback
{
public:
    void registerDMA2Stream0IRQHandler(std::function<void()> c);
    void registerDMA1Stream3IRQHandler(std::function<void()> c);
    void registerDMA1Stream4IRQHandler(std::function<void()> c);
    void registerI2c2EventIRQHandler(std::function<void()> c);
    void registerUart2IRQHandler(std::function<void()> c);
    void registerUart2RxCallback(std::function<void()> c);
    void registerUart2TxCallback(std::function<void()> c);
    void registerAdcConvCpltCallback(std::function<void()> c);
    void registerSpi2TxRxCpltCallback(std::function<void()> c);
    void deregisterSpi2TxRxCpltCallback();
    void registerI2c2TxCpltCallback(std::function<void()> c);
    void registerI2c2RxCpltCallback(std::function<void()> c);
    void invokeIRQHandler(IRQn_Type irqN);
    void invokeRxCallback(IRQn_Type irqN);
    void invokeTxCallback(IRQn_Type irqN);
    void invokeAdcConvCpltCallback();
    void invokeSpi2TxRxCpltCallback();
    static CIrqCallback& getInstance();

private:
    CIrqCallback();
    std::function<void()> mDMA2Stream0IRQHandler;
    std::function<void()> mDMA1Stream3IRQHandler;
    std::function<void()> mDMA1Stream4IRQHandler;
    std::function<void()> mI2c2EventIRQHandler;
    static std::function<void()> mUart2IRQHandler;
    static std::function<void()> mUart2RxCallback;
    static std::function<void()> mUart2TxCallback;
    std::function<void()> mAdcConvCpltCallback;
    std::function<void()> mSpi2TxRxCpltCallback;
    std::function<void()> mI2c2TxCpltCallback;
    std::function<void()> mI2c2RxCpltCallback;
};



#endif /* INC_CIRQCALLBACK_H_ */
