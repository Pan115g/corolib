/*
 * CSpiTransmitReceiveTask.cpp
 *
 *  Created on: Jun 5, 2025
 *      Author: pan.li
 */

#include <CSpiTransmitReceiveTask.h>
#include <CIrqCallback.h>
#include <CSpiMasterAdapter.h>

namespace corolib {

CSpiTransmitReceiveTask::CSpiTransmitReceiveTask(CSpiMasterAdapter &spiAdapter, const std::span<uint8_t> txData,
        std::span<uint8_t> rxData) : CAwaitableIoTask<CSpiTransmitReceiveTask>{},
            mSpiAdapter{spiAdapter}, mTxData{txData}, mRxData{rxData}
{
    CIrqCallback::getInstance().registerSpi2TxRxCpltCallback([this](){
        mCallback(this, 0);
    });
}

CSpiTransmitReceiveTask::~CSpiTransmitReceiveTask() {

}

bool CSpiTransmitReceiveTask::start()
{
    return mSpiAdapter.startTransfer(mTxData, mRxData);
}

void CSpiTransmitReceiveTask::getResult()
{
    return;
}
} /* namespace corolib */
