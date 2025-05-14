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

    bool SocketReceiveTask::start()
    {
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

        int res = ::recv(mSocket.getSocketHandle(), mBuffer.data(), mBuffer.size(), 0);
        if (res > 0)
        {
            mSkipped = true;
            mNumberOfBytesReceived = res;
            return false;
        }

        if (res == -1)
        {
            if (errno != EAGAIN && errno != EWOULDBLOCK)
            {
                const int errorCode = errno;
                throw std::system_error(
                    errorCode,
                    std::system_category(),
                    "Error receiving socket: recv() in start()");
            }
        }
        return true;
    }

    std::size_t SocketReceiveTask::getResult()
    {
        if (mSkipped)
        {
            return mNumberOfBytesReceived;
        }

        int res = ::recv(mSocket.getSocketHandle(), mBuffer.data(), mBuffer.size(), 0);
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
            mNumberOfBytesReceived = res;
        }
        return mNumberOfBytesReceived;
    }
}