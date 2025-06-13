/*
 * CI2cMasterReceiveResponseTask.h
 *
 *  Created on: Jun 13, 2025
 *      Author: pan.li
 */

#ifndef SRC_COROLIB_CI2CMASTERRECEIVERESPONSETASK_H_
#define SRC_COROLIB_CI2CMASTERRECEIVERESPONSETASK_H_

#include <span>
#include "CAwaitableIoTask.h"

namespace corolib {

class CI2cMasterAdapter;

class CI2cMasterReceiveResponseTask : public CAwaitableIoTask<CI2cMasterReceiveResponseTask> {
public:
    CI2cMasterReceiveResponseTask(CI2cMasterAdapter &i2cMaster, std::span<uint8_t> buffer);
    ~CI2cMasterReceiveResponseTask();

    bool start();
    void getResult() {};
private:
    CI2cMasterAdapter &mI2cMaster;
    std::span<uint8_t> mBuffer;
};

} /* namespace corolib */

#endif /* SRC_COROLIB_CI2CMASTERRECEIVERESPONSETASK_H_ */
