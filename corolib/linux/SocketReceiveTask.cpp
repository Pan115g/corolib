#include "SocketReceiveTask.h"
#include <system_error>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <TcpSocket.h>
#include <IoEventHandler.h>

namespace corolib
{
    SocketReceiveTask::SocketReceiveTask(IoEventHandler& mIoEventHandler, TcpSocket& socket, std::span<uint8_t> buffer) noexcept 
    : AwaitableIoTask<SocketReceiveTask>{}, 
    mIoEventHandler{mIoEventHandler},
    mSocket{socket},
    mBuffer{buffer}
    {
    }

    SocketReceiveTask::~SocketReceiveTask()
    {
        if (mSocket.isInitialized())
        {
            epoll_event ev = { 0, { 0 } };
            ev.events = IoEventHandler::Default_Events;
            if (0 != epoll_ctl(mIoEventHandler.getEpollFileDescriptor(), EPOLL_CTL_MOD, 
                                mSocket.getSocketHandle(), &ev))
            {
                mNumberOfBytesReceived = 0;
            }
        }
    }

    bool SocketReceiveTask::start()
    {        
        if (readAndFindDelimiter())
        {
            mSkipped = true;
            return false;
        }

        epoll_event ev = { 0, { 0 } };
        ev.events = IoEventHandler::Default_Events;
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

    bool SocketReceiveTask::checkResumeCondition(uint32_t events) noexcept
    {
        if ((events & EPOLLIN) == 0)
        {
            return false;
        }
        if (mSkipped)
        {
            return true;
        }

        if (readAndFindDelimiter())
        {
            mSkipped = true;
            return true;
        }
        return false;
    }

    std::size_t SocketReceiveTask::getResult()
    {
        return mNumberOfBytesReceived;
    }
        
    bool SocketReceiveTask::readAndFindDelimiter()
    {
        int res = ::recv(mSocket.getSocketHandle(), mBuffer.data() + mNumberOfBytesReceived, 
            mBuffer.size() - mNumberOfBytesReceived, 0);
        if (res == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
        {
            const int errorCode = errno;
            throw std::system_error(
                errorCode,
                std::system_category(),
                "Error receiving socket: recv() in getResult()");
        }

        if (res > 0)
        {
            mNumberOfBytesReceived += res;
        }
        return mNumberOfBytesReceived == mBuffer.size();
    }
}