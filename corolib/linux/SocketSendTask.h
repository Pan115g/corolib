#ifndef SOCKETSENDTASK_H_
#define SOCKETSENDTASK_H_

#include <span>
#include "AwaitableIoTask.h"

namespace corolib
{
    class TcpSocket;
    class IoEventHandler;

    class SocketSendTask : public AwaitableIoTask<SocketSendTask>
    {
        public:
        explicit SocketSendTask(IoEventHandler& mIoEventHandler, TcpSocket& socket, std::span<uint8_t> buffer) noexcept;

        ~SocketSendTask()
        {

        }
        
        bool start();
        std::size_t getResult();

        private:
        IoEventHandler& mIoEventHandler;
        TcpSocket& mSocket;
        std::span<uint8_t> mBuffer;
        uint32_t mNumberOfBytesSent{0};
    };
}
#endif