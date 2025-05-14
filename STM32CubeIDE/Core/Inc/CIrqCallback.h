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
    void registerUart2IRQHandler(std::function<void()> c);
    void registerUart2RxCallback(std::function<void()> c);
    void registerUart2TxCallback(std::function<void()> c);
    void invokeIRQHandler(IRQn_Type irqN);
    void invokeRxCallback(IRQn_Type irqN);
    void invokeTxCallback(IRQn_Type irqN);
    static CIrqCallback& getInstance();

private:
    CIrqCallback();
    static std::function<void()> mUart2IRQHandler;
    static std::function<void()> mUart2RxCallback;
    static std::function<void()> mUart2TxCallback;
};



#endif /* INC_CIRQCALLBACK_H_ */
