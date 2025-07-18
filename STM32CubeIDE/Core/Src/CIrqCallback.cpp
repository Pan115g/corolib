
#include "CIrqCallback.h"
#include "CUartAdapter.h"
#include "CDeviceCreator.h"

#ifdef __cplusplus
extern "C" {
#endif

extern TIM_HandleTypeDef        htim5;

void DMA2_Stream0_IRQHandler(void)
{
    CIrqCallback::getInstance().invokeIRQHandler(DMA2_Stream0_IRQn);
}

void DMA1_Stream3_IRQHandler(void)
{
    CIrqCallback::getInstance().invokeIRQHandler(DMA1_Stream3_IRQn);
}

void DMA1_Stream4_IRQHandler(void)
{
    CIrqCallback::getInstance().invokeIRQHandler(DMA1_Stream4_IRQn);
}

void USART2_IRQHandler(void)
{
    CIrqCallback::getInstance().invokeIRQHandler(USART2_IRQn);
}

void I2C2_EV_IRQHandler(void)
{
    CIrqCallback::getInstance().invokeIRQHandler(I2C2_EV_IRQn);
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

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == I2C2)
    {
        CIrqCallback::getInstance().invokeTxCallback(I2C2_EV_IRQn);
    }
}


void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == I2C2)
    {
        CIrqCallback::getInstance().invokeRxCallback(I2C2_EV_IRQn);
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1)
    {
        CIrqCallback::getInstance().invokeAdcConvCpltCallback();
    }
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPI2)
    {
        CIrqCallback::getInstance().invokeSpi2TxRxCpltCallback();
    }
}

void NMI_Handler(void)
{
   while (1)
  {
  }
}


void HardFault_Handler(void)
{
  while (1)
  {
  }
}

void MemManage_Handler(void)
{

  while (1)
  {
  }
}

void BusFault_Handler(void)
{

  while (1)
  {
  }
}


void UsageFault_Handler(void)
{
  while (1)
  {

  }
}

void DebugMon_Handler(void)
{

}

void TIM5_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim5);
}

#ifdef __cplusplus
}
#endif

void CIrqCallback::registerDMA2Stream0IRQHandler(std::function<void()> c)
{
    mDMA2Stream0IRQHandler = c;
}

void CIrqCallback::registerDMA1Stream3IRQHandler(std::function<void()> c)
{
    mDMA1Stream3IRQHandler = c;
}

void CIrqCallback::registerDMA1Stream4IRQHandler(std::function<void()> c)
{
    mDMA1Stream4IRQHandler = c;
}

void CIrqCallback::registerUart2IRQHandler(std::function<void()> c)
{
    mUart2IRQHandler = c;
}

void CIrqCallback::registerI2c2EventIRQHandler(std::function<void()> c)
{
    mI2c2EventIRQHandler = c;
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

void CIrqCallback::registerSpi2TxRxCpltCallback(std::function<void()> c)
{
    mSpi2TxRxCpltCallback = c;
}

void CIrqCallback::deregisterSpi2TxRxCpltCallback()
{
    mSpi2TxRxCpltCallback = nullptr;
}

void CIrqCallback::registerI2c2TxCpltCallback(std::function<void()> c)
{
    mI2c2TxCpltCallback = c;
}

void CIrqCallback::registerI2c2RxCpltCallback(std::function<void()> c)
{
    mI2c2RxCpltCallback = c;
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
    case DMA1_Stream3_IRQn:
        if (mDMA1Stream3IRQHandler)
        {
            mDMA1Stream3IRQHandler();
        }
        break;
    case DMA1_Stream4_IRQn:
        if (mDMA1Stream4IRQHandler)
        {
            mDMA1Stream4IRQHandler();
        }
        break;
    case I2C2_EV_IRQn:
        if (mI2c2EventIRQHandler)
        {
            mI2c2EventIRQHandler();
        }
        break;
    default:
        return;
    }
}

void CIrqCallback::invokeRxCallback(IRQn_Type irqN)
{
    switch (irqN)
    {
    case USART2_IRQn:
        if (mUart2RxCallback)
        {
            mUart2RxCallback();
        }
        break;
    case I2C2_EV_IRQn:
        if (mI2c2RxCpltCallback)
        {
            mI2c2RxCpltCallback();
        }
        break;
    default:
        break;
    }
}

void CIrqCallback::invokeTxCallback(IRQn_Type irqN)
{
    switch (irqN)
    {
    case USART2_IRQn:
        if (mUart2TxCallback)
        {
            mUart2TxCallback();
        }
        break;
    case I2C2_EV_IRQn:
        if (mI2c2TxCpltCallback)
        {
            mI2c2TxCpltCallback();
        }
        break;
    default:
        break;
    }
}

void CIrqCallback::invokeAdcConvCpltCallback()
{
    if (mAdcConvCpltCallback)
    {
        mAdcConvCpltCallback();
    }
}

void CIrqCallback::invokeSpi2TxRxCpltCallback()
{
    if (mSpi2TxRxCpltCallback)
    {
        mSpi2TxRxCpltCallback();
    }
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

