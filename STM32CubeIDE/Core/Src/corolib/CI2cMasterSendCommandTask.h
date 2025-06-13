/*
 * CI2cMasterSendCommandTask.h
 *
 *  Created on: Jun 12, 2025
 *      Author: pan.li
 */

#ifndef SRC_COROLIB_CI2CMASTERSENDCOMMANDTASK_H_
#define SRC_COROLIB_CI2CMASTERSENDCOMMANDTASK_H_

#include <span>
#include "CAwaitableIoTask.h"

namespace corolib {

class CI2cMasterAdapter;

class CI2cMasterSendCommandTask : public CAwaitableIoTask<CI2cMasterSendCommandTask>{
public:
    CI2cMasterSendCommandTask(CI2cMasterAdapter &i2cMaster, const std::span<uint8_t> buffer);
    ~CI2cMasterSendCommandTask();

    bool start();
    void getResult() {};
private:
    CI2cMasterAdapter &mI2cMaster;
    std::span<uint8_t> mBuffer;
};

} /* namespace corolib */

#endif /* SRC_COROLIB_CI2CMASTERSENDCOMMANDTASK_H_ */
