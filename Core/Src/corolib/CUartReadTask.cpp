/*
 * CUartReadTask.cpp
 *
 *  Created on: May 8, 2025
 *      Author: pan.li
 */

#include "CUartReadTask.h"
#include "CUartAdapter.h"

namespace corolib
{
    CUartReadTask::CUartReadTask(CUartAdapter &uartAdapter, std::span<uint8_t> buffer)
    : mUartAdapter{uartAdapter}, mBuffer{buffer}
    {
    }

    CUartReadTask::~CUartReadTask()
    {
    }

    bool CUartReadTask::start()
    {
        mUartAdapter.registerMessageReceived([this](const uint8_t* data, const uint32_t size){
            mNumOfBytesReceived = size;
            mCallback(this, 0);
        });

        if (mUartAdapter.read(mBuffer))
        {
            return true;
        }
        mSkipped = true;
        return false;
    }

    uint32_t CUartReadTask::getResult()
    {
        if (mSkipped)
        {
            return 0;
        }
        return mNumOfBytesReceived;
    }
}

