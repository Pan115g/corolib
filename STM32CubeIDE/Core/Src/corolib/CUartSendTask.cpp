/*
 * CUartSendTask.cpp
 *
 *  Created on: May 19, 2025
 *      Author: pan.li
 */

#include <CUartSendTask.h>

namespace corolib {

    CUartSendTask::CUartSendTask(CUartAdapter &uartAdapter, const std::span<uint8_t> buffer)
    : CAwaitableIoTask<CUartSendTask>{},
    mUartAdapter{uartAdapter}, mBuffer{buffer}, mSendData{mBuffer.begin(), buffer.size()}
    {
        //std::ranges::copy(buffer.begin(), buffer.end(), mBuffer);

    }

    bool CUartSendTask::start()
    {
        mUartAdapter.registerMessageReceived([this](const uint8_t* data, const uint32_t size){
            mCallback(this, size);
        });

        if (mUartAdapter.read(mBuffer))
        {
            return true;
        }
        mSkipped = true;
        return false;
    }

    uint32_t CUartSendTask::getResult()
    {
        if (mSkipped)
        {
            return 0;
        }
        mNumOfBytesReceived = mEvents;
        return mNumOfBytesReceived;
    }

} /* namespace corolib */
