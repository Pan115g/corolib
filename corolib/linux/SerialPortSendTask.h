#ifndef SERIALPORT_SENDTASK_H_
#define SERIALPORT_SENDTASK_H_

#include <span>
#include "AwaitableIoTask.h"

namespace corolib
{
    class SerialPort;
    class IoEventHandler;

    class SerialPortSendTask : public AwaitableIoTask<SerialPortSendTask>
    {
        public:        
        explicit SerialPortSendTask(IoEventHandler& mIoEventHandler, SerialPort& serialPort, const std::span<uint8_t> buffer) noexcept;

        ~SerialPortSendTask()
        {

        }
        
        bool start();
        std::size_t getResult();
        bool checkResumeCondition() {return true;}

        private:
        IoEventHandler& mIoEventHandler;
        SerialPort& mSerialPort;
        std::span<uint8_t> mBuffer;
        uint32_t mNumberOfBytesSent{0};
    };
}
#endif