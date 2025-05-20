/*
 * CAdcReceiveTask.h
 *
 *  Created on: May 19, 2025
 *      Author: pan.li
 */

#ifndef SRC_COROLIB_CADCRECEIVETASK_H_
#define SRC_COROLIB_CADCRECEIVETASK_H_

#include <span>
#include "CAwaitableIoTask.h"

namespace corolib {

    class CAdcAdapter;

    class CAdcReceiveTask : public CAwaitableIoTask<CAdcReceiveTask> {
    public:
        explicit CAdcReceiveTask(CAdcAdapter& adcAdapter, std::span<uint8_t> buffer);

        bool start();
        uint32_t getResult();

    private:
        CAdcAdapter& mAdcAdapter;
        std::span<uint8_t> mBuffer;
    };

} /* namespace corolib */

#endif /* SRC_COROLIB_CADCRECEIVETASK_H_ */
