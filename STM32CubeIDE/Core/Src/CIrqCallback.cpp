
#include "CIrqCallback.h"
#include "CUartAdapter.h"
#include "CDeviceCreator.h"

#ifdef __cplusplus
extern "C" {
#endif

void DMA2_Stream0_IRQHandler(void)
{
    CIrqCallback::getInstance().invokeIRQHandler(DMA2_Stream0_IRQn);
}

void USART2_IRQHandler(void)
{
    CIrqCallback::getInstance().invokeIRQHandler(USART2_IRQn);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        CIrqCallback::getInstance().invokeRxCallback(USART2_IRQn);
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        CIrqCallback::getInstance().invokeTxCallback(USART2_IRQn);
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1)
    {
        CIrqCallback::getInstance().invokeAdcConvCpltCallback();
    }
}

#ifdef __cplusplus
}
#endif

void CIrqCallback::registerDMA2Stream0IRQHandler(std::function<void()> c)
{
    mDMA2Stream0IRQHandler = c;
}

void CIrqCallback::registerUart2IRQHandler(std::function<void()> c)
{
    mUart2IRQHandler = c;
}

void CIrqCallback::registerUart2RxCallback(std::function<void()> c)
{
    mUart2RxCallback = c;
}

void CIrqCallback::registerUart2TxCallback(std::function<void()> c)
{
    mUart2TxCallback = c;
}

void CIrqCallback::registerAdcConvCpltCallback(std::function<void()> c)
{
    mAdcConvCpltCallback = c;
}

void CIrqCallback::invokeIRQHandler(IRQn_Type irqN)
{
    switch (irqN)
    {
    case USART2_IRQn:
        if (mUart2IRQHandler)
        {
            mUart2IRQHandler();
        }
        break;
    case DMA2_Stream0_IRQn:
        if (mDMA2Stream0IRQHandler)
        {
            mDMA2Stream0IRQHandler();
        }
        break;
    default:
        return;
    }
}

void CIrqCallback::invokeRxCallback(IRQn_Type irqN)
{
    if (irqN == USART2_IRQn && mUart2RxCallback)
    {
        mUart2RxCallback();
    }
}

void CIrqCallback::invokeTxCallback(IRQn_Type irqN)
{
    if (irqN == USART2_IRQn && mUart2TxCallback)
    {
        mUart2TxCallback();
    }
}

void CIrqCallback::invokeAdcConvCpltCallback()
{
    mAdcConvCpltCallback();
}

CIrqCallback& CIrqCallback::getInstance()
{
    static CIrqCallback aIrqCallback;
    return aIrqCallback;
}

std::function<void()> CIrqCallback::mUart2IRQHandler;
std::function<void()> CIrqCallback::mUart2RxCallback;
std::function<void()> CIrqCallback::mUart2TxCallback;
CIrqCallback::CIrqCallback()
{
}

