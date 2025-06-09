/*
 * CSpiTransmitReceiveTask.h
 *
 *  Created on: Jun 5, 2025
 *      Author: pan.li
 */

#ifndef SRC_COROLIB_CSPITRANSMITRECEIVETASK_H_
#define SRC_COROLIB_CSPITRANSMITRECEIVETASK_H_

#include <CAwaitableIoTask.h>
#include <span>

namespace corolib {

class CSpiMasterAdapter;

class CSpiTransmitReceiveTask : public CAwaitableIoTask<CSpiTransmitReceiveTask> {
public:
    CSpiTransmitReceiveTask(CSpiMasterAdapter &spiAdapter, const std::span<uint8_t> txData, std::span<uint8_t> rxData);
    ~CSpiTransmitReceiveTask();

    bool start();
    void getResult();

private:
    CSpiMasterAdapter &mSpiAdapter;
    std::span<uint8_t> mTxData;
    std::span<uint8_t> mRxData;
};

} /* namespace corolib */

#endif /* SRC_COROLIB_CSPITRANSMITRECEIVETASK_H_ */
