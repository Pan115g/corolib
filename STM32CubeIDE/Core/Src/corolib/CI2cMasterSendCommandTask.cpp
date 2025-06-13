/*
 * CI2cMasterSendCommandTask.cpp
 *
 *  Created on: Jun 12, 2025
 *      Author: pan.li
 */

#include <CI2cMasterSendCommandTask.h>
#include <CIrqCallback.h>
#include <CI2cMasterAdapter.h>

namespace corolib {

CI2cMasterSendCommandTask::CI2cMasterSendCommandTask(CI2cMasterAdapter &i2cMaster, const std::span<uint8_t> buffer)
        : CAwaitableIoTask<CI2cMasterSendCommandTask>{}, mI2cMaster{i2cMaster}, mBuffer{buffer} {

            CIrqCallback::getInstance().registerI2c2TxCpltCallback([this](){
                mCallback(this, 0);
            });

}

CI2cMasterSendCommandTask::~CI2cMasterSendCommandTask() {
    // TODO Auto-generated destructor stub
}

bool CI2cMasterSendCommandTask::start()
{
    return mI2cMaster.write(mBuffer);
}


} /* namespace corolib */
