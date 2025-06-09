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
#include "CRtcAdapter.h"
#include "CSpiMasterAdapter.h"
#include "CIrqCallback.h"
#include "CUartReadTask.h"
#include "CAdcReceiveTask.h"
#include "CSpiTransmitReceiveTask.h"
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
    int res = std::snprintf(sendBuffer, sizeof(sendBuffer), "Cpu temperature: %3ld\r", (int32_t)temperatureSum);
    co_await writeUart(scheduler, uart, {(uint8_t*)sendBuffer, 21UL});
}

corolib::Awaitable<> transferSpi(corolib::CTaskScheduler &scheduler, corolib::CSpiMasterAdapter& spiMaster,
        corolib::CRtcAdapter& rtc, corolib::CUartAdapter& uart)
{
    uint8_t receiveBuffer[32] = {0};
    uint8_t sendBuffer[32] = {0};
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t weekDay;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    const uint8_t payloadSize = 9U;
    static uint8_t messageId = 0;

    if (!rtc.getDateTime(year, month, day, weekDay, hour, minute, second))
    {
        co_return;
    }
    sendBuffer[0] = payloadSize;
    sendBuffer[1] = ++messageId;
    sendBuffer[2] = year;
    sendBuffer[3] = month;
    sendBuffer[4] = day;
    sendBuffer[5] = weekDay;
    sendBuffer[6] = hour;
    sendBuffer[7] = minute;
    sendBuffer[8] = second;

    co_await corolib::CSpiTransmitReceiveTask(spiMaster, {sendBuffer, 32}, {receiveBuffer, 32});
    if (receiveBuffer[0] == 0)
    {
        co_return;
    }

    const char err_payloadsize[] = "slave payload size wrong\r";
    const uint32_t err_payloadsize_len = strlen(err_payloadsize);
    const char err_msgid[] = "slave message id wrong\r";
    const uint32_t err_msgid_len = strlen(err_msgid);
    const char err_date[] = "slave date wrong\r";
    const uint32_t err_date_len = strlen(err_date);
    const char err_time[] = "slave time wrong\r";
    const uint32_t err_time_len = strlen(err_time);

    if (receiveBuffer[0] != payloadSize)
    {
        co_await writeUart(scheduler, uart, {(uint8_t*)err_payloadsize, err_payloadsize_len});
    }

    if (receiveBuffer[1] != (messageId - 1))
    {
        co_await writeUart(scheduler, uart, {(uint8_t*)err_msgid, err_msgid_len});
    }

    if (receiveBuffer[2] != year || receiveBuffer[3] != month || receiveBuffer[4] != day ||
            receiveBuffer[5] != weekDay)
    {
        co_await writeUart(scheduler, uart, {(uint8_t*)err_date, err_date_len});
    }

    if (receiveBuffer[6] != hour || receiveBuffer[7] != minute || receiveBuffer[8] != second)
    {
        char err_time_s[128];
        std::snprintf(err_time_s, sizeof(err_time_s), "wrong time: %2d %2d %2d, %2d %2d %2d\r",
                receiveBuffer[6], receiveBuffer[7], receiveBuffer[8], hour, minute, second);
        co_await writeUart(scheduler, uart, {(uint8_t*)err_time_s, strlen(err_time_s)});
    }
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
    sTimerAdc = xTimerCreate ("ac_timer", pdMS_TO_TICKS(10000), pdTRUE, NULL, CMainTask::adcTimerCallback);

    sOnTimeout = [this](){
        //corolib::fireAndForget(readAdc(sScheduler, CDeviceCreator::getInstance().getAdc1(), CDeviceCreator::getInstance().getUart2()));
        corolib::fireAndForget(transferSpi(sScheduler, CDeviceCreator::getInstance().getSpi2Master(),
                CDeviceCreator::getInstance().getRtc(), CDeviceCreator::getInstance().getUart2()));
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
