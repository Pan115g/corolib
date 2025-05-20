/*
 * CUartSendTask.h
 *
 *  Created on: May 19, 2025
 *      Author: pan.li
 */

#ifndef SRC_COROLIB_CUARTSENDTASK_H_
#define SRC_COROLIB_CUARTSENDTASK_H_

#include <span>
#include "CAwaitableIoTask.h"

namespace corolib {

    class CUartAdapter;

    class CUartSendTask : public CAwaitableIoTask<CUartSendTask> {
    public:
        static constexpr uint32_t cMaxBufferSize = 128UL;
        CUartSendTask(CUartAdapter &uartAdapter, const std::span<uint8_t> buffer);

        bool start();
        uint32_t getResult();
    private:
        std::array<uint8_t, cMaxBufferSize> mBuffer;
        std::span<uint8_t> mSendData;
    };

} /* namespace corolib */

#endif /* SRC_COROLIB_CUARTSENDTASK_H_ */
