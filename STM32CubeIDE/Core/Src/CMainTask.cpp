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
#include <atomic>
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
#include "CI2cMasterSendCommandTask.h"
#include "CI2cMasterSendRequestTask.h"
#include "CI2cMasterReceiveResponseTask.h"
#include "CTaskScheduler.h"
#include "CDeviceCreator.h"

#ifdef DEBUG
#include <stdio.h>
#include "CTrace.h"
#endif

static std::atomic<bool> sPrintAdc{};
static std::atomic<bool> sPrintSpi{};

corolib::Awaitable<> transferI2cCommand(corolib::CTaskScheduler &scheduler, corolib::CI2cMasterAdapter& i2cMaster,
        const std::span<uint8_t> send_data)
{
    i2cMaster.setSlaveAddress(38U);

    co_await corolib::CI2cMasterSendCommandTask(i2cMaster, send_data);
}

corolib::Awaitable<> transferI2cRequest(corolib::CTaskScheduler &scheduler, corolib::CI2cMasterAdapter& i2cMaster,
        const std::span<uint8_t> send_data, std::span<uint8_t> receive_buffer)
{
    i2cMaster.setSlaveAddress(38U);

    co_await corolib::CI2cMasterSendRequestTask(i2cMaster, send_data);
    co_await corolib::CI2cMasterReceiveResponseTask(i2cMaster, receive_buffer);
}

corolib::Awaitable<> writeUart(corolib::CTaskScheduler &scheduler, corolib::CUartAdapter& uart, const std::span<uint8_t> buffer)
{
    std::array<uint8_t, 128> buffer_loc;
    uint32_t data_size = buffer.size();
    std::ranges::copy(buffer.begin(), buffer.end(), buffer_loc.begin());
    co_await scheduler.schedule();
    HAL_UART_Transmit(&uart.mUart,(uint8_t*)buffer_loc.data(), data_size, HAL_MAX_DELAY);
}

corolib::Awaitable<> readUart(corolib::CTaskScheduler &scheduler, corolib::CUartAdapter& uart, corolib::CRtcAdapter &rtc)
{
    uint8_t send_data[32] = {0};
    uint8_t receive_buffer[32] = {0};
    char command_s[128] = "";
    const char user_data[] = "Testing coroutines\r";
    HAL_UART_Transmit(&uart.mUart,(uint8_t*)user_data,strlen(user_data),HAL_MAX_DELAY);
    while(true)
    {
        uint32_t numOfBytes = co_await corolib::CUartReadTask(uart, receive_buffer);
        std::snprintf(command_s, sizeof(command_s), "Given command: %2d\r", receive_buffer[0]);
        co_await writeUart(scheduler, uart, {(uint8_t*)command_s, strlen(command_s)});

        switch (receive_buffer[0])
        {
        case 0: // not print adc and spi
            sPrintAdc.store(false, std::memory_order_relaxed);
            sPrintSpi.store(false, std::memory_order_relaxed);
            break;
        case 1: // print adc and spi
            sPrintAdc.store(true, std::memory_order_relaxed);
            break;
        case 2: // print adc and spi
            sPrintSpi.store(true, std::memory_order_relaxed);
            break;
        case 0x31: //request rtc
            send_data[0] = 0x31U;
            co_await transferI2cRequest(scheduler, CDeviceCreator::getInstance().getI2c2Master(), {send_data, 32},
                    {receive_buffer, 7});
            std::snprintf(command_s, sizeof(command_s), "slave rtc: %2d %2d %2d %2d, %2d %2d %2d\r",
                    receive_buffer[0], receive_buffer[1], receive_buffer[2], receive_buffer[3], receive_buffer[4],
                    receive_buffer[5], receive_buffer[6]);
            co_await writeUart(scheduler, uart, {(uint8_t*)command_s, strlen(command_s)});
            break;
        case 0x32: //reset rtc
        {
            uint8_t year;
            uint8_t month;
            uint8_t day;
            uint8_t weekDay;
            uint8_t hour;
            uint8_t minute;
            uint8_t second;
            if (!rtc.getDateTime(year, month, day, weekDay, hour, minute, second))
            {
                co_return;
            }
            send_data[0] = 0x32U;
            send_data[1] = year;
            send_data[2] = month;
            send_data[3] = day;
            send_data[4] = weekDay;
            send_data[5] = hour;
            send_data[6] = minute;
            send_data[7] = second;
            co_await transferI2cCommand(scheduler, CDeviceCreator::getInstance().getI2c2Master(), {send_data, 32});
        }
            break;
        case 0x33: //request cpu temperature
        {
            send_data[0] = 0x33U;
            co_await transferI2cRequest(scheduler, CDeviceCreator::getInstance().getI2c2Master(), {send_data, 32},
                    {receive_buffer, 4});
            float slave_cpu_temperature = 0;
            uint8_t * p_temperature = (uint8_t*)(&slave_cpu_temperature);
            p_temperature[0] = receive_buffer[0];
            p_temperature[1] = receive_buffer[1];
            p_temperature[2] = receive_buffer[2];
            p_temperature[3] = receive_buffer[3];
            std::snprintf(command_s, sizeof(command_s), "slave temperature: %2d\r", (int)slave_cpu_temperature);
            co_await writeUart(scheduler, uart, {(uint8_t*)command_s, strlen(command_s)});
        }
            break;
        default:
            break;
        }

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
    if (sPrintAdc.load(std::memory_order_relaxed))
    {
        co_await writeUart(scheduler, uart, {(uint8_t*)sendBuffer, 21UL});
    }
}

corolib::Awaitable<> transferSpi(corolib::CTaskScheduler &scheduler, corolib::CSpiMasterAdapter& spiMaster,
        corolib::CRtcAdapter& rtc, corolib::CUartAdapter& uart)
{
    uint8_t receiveBuffer[32] = {0};
    uint8_t sendBuffer[32] = {0};
    const uint8_t payloadSize = 2U;
    static uint8_t messageId = 0;

    sendBuffer[0] = payloadSize;
    sendBuffer[1] = ++messageId;

    co_await corolib::CSpiTransmitReceiveTask(spiMaster, {sendBuffer, 32}, {receiveBuffer, 32});
    if (receiveBuffer[0] == 0)
    {
        co_return;
    }

    if (sPrintSpi.load(std::memory_order_relaxed))
    {
        const char err_payloadsize[] = "slave payload size wrong\r";
        const uint32_t err_payloadsize_len = strlen(err_payloadsize);
        const char err_msgid[] = "slave message id wrong\r";
        const uint32_t err_msgid_len = strlen(err_msgid);
        if (receiveBuffer[0] != 6U)
        {
            co_await writeUart(scheduler, uart, {(uint8_t*)err_payloadsize, err_payloadsize_len});
        }

        if (receiveBuffer[1] != (messageId - 1))
        {
            co_await writeUart(scheduler, uart, {(uint8_t*)err_msgid, err_msgid_len});
        }

        uint32_t* cputicks = (uint32_t*)&receiveBuffer[2];
        char cpu_ticks_s[128];
        std::snprintf(cpu_ticks_s, sizeof(cpu_ticks_s), "Slave cpu ticks: %lu\r", *cputicks);
        co_await writeUart(scheduler, uart, {(uint8_t*)cpu_ticks_s, strlen(cpu_ticks_s)});
    }
}

std::function<void()> CMainTask::sOnAdcTimeout{};
std::function<void()> CMainTask::sOnSpiTimeout{};
CMainTask::CMainTask() : mScheduler{}
#ifdef DEBUG
    , mTrace{}
#endif
    {

    const osThreadAttr_t defaultTask_attributes = {
      .name = "MainTask",
      .stack_size = 128 * 8,
      .priority = (osPriority_t) osPriorityNormal,
    };
    mMainThread = osThreadNew(CMainTask::runThread, (void*)this, &defaultTask_attributes);
    mTimerAdc = xTimerCreate ("adc_timer", pdMS_TO_TICKS(1000), pdTRUE, NULL, CMainTask::adcTimerCallback);
    mTimerSpi = xTimerCreate ("spi_timer", pdMS_TO_TICKS(10), pdTRUE, NULL, CMainTask::spiTimerCallback);

    sOnAdcTimeout = [this](){
        corolib::fireAndForget(readAdc(mScheduler, CDeviceCreator::getInstance().getAdc1(), CDeviceCreator::getInstance().getUart2()));
    };
    sOnSpiTimeout = [this](){
        corolib::fireAndForget(transferSpi(mScheduler, CDeviceCreator::getInstance().getSpi2Master(),
               CDeviceCreator::getInstance().getRtc(), CDeviceCreator::getInstance().getUart2()));
    };
}

void CMainTask::runThread(void* argument)
{
    CMainTask* task = static_cast<CMainTask*>(argument);
    corolib::fireAndForget(readUart(task->mScheduler, CDeviceCreator::getInstance().getUart2(),
            CDeviceCreator::getInstance().getRtc()));
    xTimerStart(task->mTimerAdc, portMAX_DELAY);
    xTimerStart(task->mTimerSpi, portMAX_DELAY);
    //Work around for ITM. If without this line, nothing is printed in the console.
    printf("Main task \n");
    while(1)
    {
        osDelay(osWaitForever);
    }
}

void CMainTask::adcTimerCallback(TimerHandle_t xTimer)
{
    sOnAdcTimeout();
}

void CMainTask::spiTimerCallback(TimerHandle_t xTimer)
{
    sOnSpiTimeout();
}
