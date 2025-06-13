/*
 * CI2cMasterSendRequestTask.cpp
 *
 *  Created on: Jun 12, 2025
 *      Author: pan.li
 */

#include <CI2cMasterSendRequestTask.h>
#include <CIrqCallback.h>
#include <CI2cMasterAdapter.h>

namespace corolib {

CI2cMasterSendRequestTask::CI2cMasterSendRequestTask(CI2cMasterAdapter &i2cMaster, const std::span<uint8_t> buffer)
        : CAwaitableIoTask<CI2cMasterSendRequestTask>{}, mI2cMaster{i2cMaster}, mBuffer{buffer} {

            CIrqCallback::getInstance().registerI2c2TxCpltCallback([this](){
                mCallback(this, 0);
            });
}

CI2cMasterSendRequestTask::~CI2cMasterSendRequestTask() {
    // TODO Auto-generated destructor stub
}

bool CI2cMasterSendRequestTask::start()
{
    return mI2cMaster.writeFirstFrame(mBuffer);
}
} /* namespace corolib */
