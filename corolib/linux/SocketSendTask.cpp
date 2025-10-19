#include "SocketSendTask.h"
#include <system_error>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <TcpSocket.h>
#include <IoEventHandler.h>

namespace corolib
{
    SocketSendTask::SocketSendTask(IoEventHandler& mIoEventHandler, TcpSocket& socket, const std::span<uint8_t> buffer) noexcept 
    : AwaitableIoTask<SocketSendTask>{}, 
    mIoEventHandler{mIoEventHandler},
    mSocket{socket},
    mBuffer{buffer}
    {
    }

    SocketSendTask::~SocketSendTask()
    {
        if (mSocket.isInitialized())
        {
            epoll_event ev = { 0, { 0 } };
            ev.events = IoEventHandler::Default_Events & ~EPOLLOUT;
            if (0 != epoll_ctl(mIoEventHandler.getEpollFileDescriptor(), EPOLL_CTL_MOD, 
                                mSocket.getSocketHandle(), &ev))
            {
                mNumberOfBytesSent = 0;
            }
        }
    }

    bool SocketSendTask::start()
    {        
        if (sendAndCheckNumberOfSentBytes())
        {
            mSkipped = true;
            return false;
        }

        epoll_event ev = { 0, { 0 } };
        ev.events = IoEventHandler::Default_Events | EPOLLOUT;
        ev.data.ptr = this;
        if (0 != epoll_ctl(mIoEventHandler.getEpollFileDescriptor(), EPOLL_CTL_MOD, 
                            mSocket.getSocketHandle(), &ev))
        {
            const int errorCode = errno;
            throw std::system_error(
                errorCode,
                std::system_category(),
                "Error registering socket with epoll");
        }
        return true;
    }

    bool SocketSendTask::checkResumeCondition(uint32_t events) noexcept
    {
        if ((events & EPOLLOUT) == 0)
        {
            return false;
        }

        if (mSkipped)
        {
            return true;
        }

        mSkipped = sendAndCheckNumberOfSentBytes();
        return mSkipped;
    }

    std::size_t SocketSendTask::getResult()
    {
        return mNumberOfBytesSent;
    }
    
    bool SocketSendTask::sendAndCheckNumberOfSentBytes()
    {        
        int res = ::send(mSocket.getSocketHandle(), mBuffer.data() + mNumberOfBytesSent, mBuffer.size() - mNumberOfBytesSent, 0);
        if (res == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
        {
            const int errorCode = errno;
            throw std::system_error(
                errorCode,
                std::system_category(),
                "Error receiving socket: send() in getResult()");
        }

        if (res > 0)
        {
            mNumberOfBytesSent += res;
        }

        return mNumberOfBytesSent == mBuffer.size();
    }
}