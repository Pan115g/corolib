/*
 * CI2cMasterReceiveResponseTask.cpp
 *
 *  Created on: Jun 13, 2025
 *      Author: pan.li
 */

#include <CI2cMasterReceiveResponseTask.h>
#include <CIrqCallback.h>
#include <CI2cMasterAdapter.h>

namespace corolib {

CI2cMasterReceiveResponseTask::CI2cMasterReceiveResponseTask(CI2cMasterAdapter &i2cMaster, std::span<uint8_t> buffer)
    : CAwaitableIoTask<CI2cMasterReceiveResponseTask>{}, mI2cMaster{i2cMaster}, mBuffer{buffer} {

        CIrqCallback::getInstance().registerI2c2RxCpltCallback([this](){
            mCallback(this, 0);
        });
}

CI2cMasterReceiveResponseTask::~CI2cMasterReceiveResponseTask() {
    // TODO Auto-generated destructor stub
}

bool CI2cMasterReceiveResponseTask::start()
{
    return mI2cMaster.readLastFrame(mBuffer);
}

} /* namespace corolib */
