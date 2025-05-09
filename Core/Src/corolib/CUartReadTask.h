/*
 * CUartReadTask.h
 *
 *  Created on: May 8, 2025
 *      Author: pan.li
 */

#ifndef SRC_COROLIB_CUARTREADTASK_H_
#define SRC_COROLIB_CUARTREADTASK_H_

#include <span>
#include "CAwaitableIoTask.h"

namespace corolib {

    class CUartAdapter;
    class CTaskScheduler;

    class CUartReadTask : public CAwaitableIoTask<CUartReadTask>
    {
    public:
        explicit CUartReadTask(CTaskScheduler& scheduler, CUartAdapter &uartAdapter, std::span<uint8_t> buffer);
        virtual ~CUartReadTask();

        bool start();
        uint32_t getResult();

    private:
        CUartAdapter &mUartAdapter;
        std::span<uint8_t> mBuffer;
        uint32_t mNumOfBytesReceived{0};
    };
}

#endif /* SRC_COROLIB_CUARTREADTASK_H_ */
