/*
 * CMainTask.h
 *
 *  Created on: May 21, 2025
 *      Author: pan.li
 */

#ifndef SRC_CMAINTASK_H_
#define SRC_CMAINTASK_H_

#include <functional>
#include "cmsis_os.h"
#include "timers.h"
#include "CTaskScheduler.h"
#ifdef DEBUG
#include "CTrace.h"
#endif

class CMainTask {
public:
    CMainTask();

private:
    static void runThread(void*) noexcept;
    static void adcTimerCallback(TimerHandle_t xTimer);
    static void spiTimerCallback(TimerHandle_t xTimer);

    static std::function<void()> sOnAdcTimeout;
    static std::function<void()> sOnSpiTimeout;
    corolib::CTaskScheduler mScheduler;
    osThreadId_t mMainThread;
    TimerHandle_t mTimerAdc;
    TimerHandle_t mTimerSpi;

#ifdef DEBUG
    CTrace mTrace;
#endif
};

#endif /* SRC_CMAINTASK_H_ */
