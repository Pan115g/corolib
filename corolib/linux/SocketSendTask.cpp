#include "SocketSendTask.h"
#include <system_error>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <TcpSocket.h>
#include <IoEventHandler.h>

namespace corolib
{
    SocketSendTask::SocketSendTask(IoEventHandler& mIoEventHandler, TcpSocket& socket, std::span<uint8_t> buffer) noexcept 
    : AwaitableIoTask<SocketSendTask>{}, 
    mIoEventHandler{mIoEventHandler},
    mSocket{socket},
    mBuffer{buffer}
    {
    }

    bool SocketSendTask::start()
    {
        epoll_event ev = { 0, { 0 } };
        ev.events = TcpSocket::Default_Events | EPOLLOUT;
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

        int res = ::send(mSocket.getSocketHandle(), mBuffer.data(), mBuffer.size(), 0);
        if (res > 0)
        {
            mSkipped = true;
            mNumberOfBytesSent = res;
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
                    "Error receiving socket: send() in start()");
            }
        }
        return true;
    }

    std::size_t SocketSendTask::getResult()
    {
        if (mSkipped)
        {
            return mNumberOfBytesSent;
        }

        int res = ::send(mSocket.getSocketHandle(), mBuffer.data(), mBuffer.size(), 0);
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
            mNumberOfBytesSent = res;
        }
        return mNumberOfBytesSent;
    }
}