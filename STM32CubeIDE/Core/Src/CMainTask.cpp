/*
 * CMainTask.cpp
 *
 *  Created on: May 21, 2025
 *      Author: pan.li
 */

#include "CMainTask.h"
#include "cmsis_os.h"
#include "timers.h"
#include <string.h>
#include <array>
#include "Awaitable.h"
#include "FireAndForget.h"
#include "CUartAdapter.h"
#include "CAdcAdapter.h"
#include "CIrqCallback.h"
#include "CUartReadTask.h"
#include "CAdcReceiveTask.h"
#include "CTaskScheduler.h"
#include "CDeviceCreator.h"

#ifdef DEBUG
#include <stdio.h>
#include "CTrace.h"
#endif


corolib::Awaitable<> writeUart(corolib::CTaskScheduler &scheduler, corolib::CUartAdapter& uart, const std::span<uint8_t> buffer)
{
    std::array<uint8_t, 128> buffer_loc;
    uint32_t data_size = buffer.size();
    std::ranges::copy(buffer.begin(), buffer.end(), buffer_loc.begin());
    co_await scheduler.schedule();
    HAL_UART_Transmit(&uart.mUart,(uint8_t*)buffer_loc.data(), data_size, HAL_MAX_DELAY);
}

corolib::Awaitable<> readUart(corolib::CTaskScheduler &scheduler, corolib::CUartAdapter& uart)
{
    std::array<uint8_t, 128> buffer_loc;
    char user_data[] = "Testing coroutines\r";
    HAL_UART_Transmit(&uart.mUart,(uint8_t*)user_data,strlen(user_data),HAL_MAX_DELAY);
    while(true)
    {
        uint32_t numOfBytes = co_await corolib::CUartReadTask(uart, buffer_loc);
        co_await writeUart(scheduler, uart, {(uint8_t*)buffer_loc.data(), numOfBytes});
    }
}


corolib::Awaitable<> readAdc(corolib::CTaskScheduler &scheduler, corolib::CAdcAdapter& adc, corolib::CUartAdapter& uart)
{
    std::array<uint8_t, 32> buffer_loc;
    uint16_t temperature = 0;
    float temperatureSum = 0;
    char sendBuffer[32];
    uint32_t numOfBytes = co_await corolib::CAdcReceiveTask(adc, buffer_loc);
    for (int i = 0; i < 16; ++i)
    {
        temperature = buffer_loc[2*i] | buffer_loc[2*i+1] << 8;
        temperatureSum += (((float)temperature * 3.3/4096.0) - 0.76)/2.5 + 25.0;

    }
    temperatureSum /= 16.0;
    int res = std::snprintf(sendBuffer, sizeof(sendBuffer), "Cpu temperature: %3d\r", (int32_t)temperatureSum);
    co_await writeUart(scheduler, uart, {(uint8_t*)sendBuffer, 21UL});
}

std::function<void()> CMainTask::sOnTimeout{};
CMainTask::CMainTask() : sScheduler{}
#ifdef DEBUG
    , mTrace{}
#endif
    {

    const osThreadAttr_t defaultTask_attributes = {
      .name = "MainTask",
      .stack_size = 128 * 6,
      .priority = (osPriority_t) osPriorityNormal,
    };
    sMainThread = osThreadNew(CMainTask::runThread, (void*)this, &defaultTask_attributes);
    sTimerAdc = xTimerCreate ("ac_timer", pdMS_TO_TICKS(10), pdTRUE, NULL, CMainTask::adcTimerCallback);

    sOnTimeout = [this](){
        corolib::fireAndForget(readAdc(sScheduler, CDeviceCreator::getInstance().getAdc1(), CDeviceCreator::getInstance().getUart2()));
    };
}

void CMainTask::runThread(void* argument)
{
    CMainTask* task = static_cast<CMainTask*>(argument);
    corolib::fireAndForget(readUart(task->sScheduler, CDeviceCreator::getInstance().getUart2()));
    BaseType_t res = xTimerStart(task->sTimerAdc, portMAX_DELAY);
    //Work around for ITM. If without this line, nothing is printed in the console.
    printf("Main task \n");
    while(1)
    {
        osDelay(osWaitForever);
    }
}

void CMainTask::adcTimerCallback(TimerHandle_t xTimer)
{
    sOnTimeout();
}
