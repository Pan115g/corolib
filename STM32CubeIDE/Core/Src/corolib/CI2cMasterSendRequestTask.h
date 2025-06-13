/*
 * CI2cMasterSendRequestTask.h
 *
 *  Created on: Jun 12, 2025
 *      Author: pan.li
 */

#ifndef SRC_COROLIB_CI2CMASTERSENDREQUESTTASK_H_
#define SRC_COROLIB_CI2CMASTERSENDREQUESTTASK_H_

#include <span>
#include "CAwaitableIoTask.h"


namespace corolib {

class CI2cMasterAdapter;

class CI2cMasterSendRequestTask : public CAwaitableIoTask<CI2cMasterSendRequestTask>{
public:
    CI2cMasterSendRequestTask(CI2cMasterAdapter &i2cMaster, const std::span<uint8_t> buffer);
    ~CI2cMasterSendRequestTask();

    bool start();
    void getResult() {};
private:
    CI2cMasterAdapter &mI2cMaster;
    std::span<uint8_t> mBuffer;
};

} /* namespace corolib */

#endif /* SRC_COROLIB_CI2CMASTERSENDREQUESTTASK_H_ */
