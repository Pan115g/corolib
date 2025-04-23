#ifndef SOCKETACCEPTTASK_H_    
#define SOCKETACCEPTTASK_H_

#include "SocketAddressConverter.h"
#include "AwaitableIoTask.h"

namespace corolib
{
    class TcpSocket;
    class IoEventHandler;

    class SocketAcceptTask : public AwaitableIoTask<SocketAcceptTask>
    {
        public:
        explicit SocketAcceptTask(IoEventHandler& mIoEventHandler, TcpSocket& acceptingSocket, TcpSocket& acceptedSocket) noexcept;

        ~SocketAcceptTask()
        {

        }
        
        bool start();

        void getResult();

        private:
        IoEventHandler& mIoEventHandler;
        TcpSocket& mAcceptingSocket;
        TcpSocket& mAcceptedSocket;
    };
}

#endif