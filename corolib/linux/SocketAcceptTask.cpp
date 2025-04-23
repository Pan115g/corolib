#include "SocketAcceptTask.h"
#include <system_error>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <TcpSocket.h>
#include <IoEventHandler.h>

namespace corolib
{
    SocketAcceptTask::SocketAcceptTask(IoEventHandler& mIoEventHandler, TcpSocket& acceptingSocket, TcpSocket& acceptedSocket) noexcept 
    : AwaitableIoTask<SocketAcceptTask>{}, 
    mIoEventHandler{mIoEventHandler},
    mAcceptingSocket{acceptingSocket},
    mAcceptedSocket{acceptedSocket}
    {
    }

    bool SocketAcceptTask::start()
    {
        epoll_event ev = { 0, { 0 } };
        ev.events = TcpSocket::Default_Events;
        ev.data.ptr = this;
        if (0 != epoll_ctl(mIoEventHandler.getEpollFileDescriptor(), EPOLL_CTL_MOD, 
                            mAcceptingSocket.getSocketHandle(), &ev))
        {
            const int errorCode = errno;
            throw std::system_error(
                errorCode,
                std::system_category(),
                "Error registering socket with epoll");
        }

        int res = ::accept(mAcceptingSocket.getSocketHandle(), nullptr, nullptr);
        if (res != -1)
        {
            mAcceptedSocket.setSocketHandle(res);
            return false;
        }

        if (errno != EAGAIN && errno != EWOULDBLOCK)
        {
            const int errorCode = errno;
            throw std::system_error(
                errorCode,
                std::system_category(),
                "Error accepting socket: accept()");
        }
        return true;
    }

    void SocketAcceptTask::getResult()
    {            
        sockaddr localSockaddr;
        sockaddr remoteSockaddr;
        socklen_t localSockaddrLen = sizeof(localSockaddr);
        socklen_t remoteSockaddrLen = sizeof(remoteSockaddr);


        int acceptedSocket = ::accept(mAcceptingSocket.getSocketHandle(), &localSockaddr, &localSockaddrLen);
        if (acceptedSocket == -1)   
        {
            const int errorCode = errno ;
            throw std::system_error{
                errorCode,
                std::system_category(),
                "Socket accept failed: accept()"
            };
        }
        mAcceptedSocket.setSocketHandle(acceptedSocket);

        uint8_t localIp[4];
        uint16_t localPort;
        bzero(&localSockaddr, sizeof(localSockaddr));
        if (::getsockname(mAcceptingSocket.getSocketHandle(), &localSockaddr, &localSockaddrLen) == 0)
        {
            convertToIpv4Address(localSockaddr, localIp, localPort);
        }
        bzero(&localSockaddr, sizeof(localSockaddr));
        if (::getsockname(mAcceptedSocket.getSocketHandle(), &localSockaddr, &localSockaddrLen) == 0)
        {
            convertToIpv4Address(localSockaddr, localIp, localPort);
        }
    }
}
