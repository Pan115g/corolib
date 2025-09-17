#ifndef SOCKETRECEIVETASK_H_
#define SOCKETRECEIVETASK_H_

#include <span>
#include "AwaitableIoTask.h"

namespace corolib
{
    class TcpSocket;
    class IoEventHandler;

    class SocketReceiveTask : public AwaitableIoTask<SocketReceiveTask>
    {
        public:
        explicit SocketReceiveTask(IoEventHandler& mIoEventHandler, TcpSocket& socket, std::span<uint8_t> buffer) noexcept;

        ~SocketReceiveTask();
        
        bool start();
        std::size_t getResult();
        bool checkResumeCondition(uint32_t events) noexcept;
        
        private:
        bool readAndFindDelimiter();
        IoEventHandler& mIoEventHandler;
        TcpSocket& mSocket;
        std::span<uint8_t> mBuffer;
        uint32_t mNumberOfBytesReceived{0};
    };
}
#endif