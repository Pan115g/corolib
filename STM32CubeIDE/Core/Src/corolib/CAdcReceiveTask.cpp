/*
 * CAdcReceiveTask.cpp
 *
 *  Created on: May 19, 2025
 *      Author: pan.li
 */

#include <CAdcReceiveTask.h>
#include "CAdcAdapter.h"
#include "CIrqCallback.h"

namespace corolib {

    CAdcReceiveTask::CAdcReceiveTask(CAdcAdapter& adcAdapter, std::span<uint8_t> buffer)
    : CAwaitableIoTask<CAdcReceiveTask>{}, mAdcAdapter{adcAdapter},
    mBuffer{buffer} {

        CIrqCallback::getInstance().registerAdcConvCpltCallback([this](){
            mCallback(this, 0);
        });
    }

    bool CAdcReceiveTask::start()
    {
        return mAdcAdapter.startConversion(mBuffer);
    }

    uint32_t CAdcReceiveTask::getResult()
    {
        return 2UL * mAdcAdapter.getNumberOfChannels();
    }
} /* namespace corolib */
