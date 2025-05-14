#ifndef SERIALPORT_RECEIVETASK_H_
#define SERIALPORT_RECEIVETASK_H_

#include <span>
#include "AwaitableIoTask.h"

namespace corolib
{
    class SerialPort;
    class IoEventHandler;

    class SerialPortReceiveTask : public AwaitableIoTask<SerialPortReceiveTask>
    {
        public:
        struct ReturnType
        {
            uint32_t bytesReceived;
            uint32_t delimiterIndex;
        };
        
        explicit SerialPortReceiveTask(IoEventHandler& mIoEventHandler, SerialPort& serialPort, std::span<uint8_t> buffer, 
            const uint8_t delimiter) noexcept;

        SerialPortReceiveTask(const SerialPortReceiveTask&) = delete;
        ~SerialPortReceiveTask()
        {

        }
        
        bool start();
        ReturnType getResult();
        bool checkResumeCondition() noexcept;

        private:
        bool readAndFindDelimiter();
        IoEventHandler& mIoEventHandler;
        SerialPort& mSerialPort;
        std::span<uint8_t> mBuffer;
        uint32_t mBufferIndex{0};
        uint32_t mNumberOfBytesReceived{0};
        uint8_t mDelimiter{0};
    };
}
#endif