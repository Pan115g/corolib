#include "TcpSocket.h"
#include <stdint.h>
#include <errno.h>
#include <system_error>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <fcntl.h>
#include "SocketAddressConverter.h"

namespace corolib
{
    TcpSocket::TcpSocket(IoEventHandler& ioEventHandler) : mIoEventHandler{ioEventHandler}, mSocketHandle{-1}
    {
        int socketFamily = AF_INET;
        auto protocol = IPPROTO_TCP;
        int socketType = SOCK_STREAM;

        int s = ::socket(socketFamily, socketType, protocol);
        int err_socket = errno;
        if (s == -1)
        {
            throw std::system_error(
                err_socket,
                std::system_category(),
                "Error creating socket");
        }
        mSocketHandle = s;

        epoll_event ev = { 0, { 0 } };
        ev.events = Default_Events;
        if (0 != epoll_ctl(ioEventHandler.getEpollFileDescriptor(), EPOLL_CTL_ADD, mSocketHandle, &ev))
        {
            const int errorCode = errno;
            throw std::system_error(
                errorCode,
                std::system_category(),
                "Error registering socket with epoll");
        }

        mInitialized = true;
    }
    
    TcpSocket::TcpSocket(IoEventHandler& ioEventHandler, int socketHandle) 
    : mIoEventHandler{ioEventHandler}, mSocketHandle{socketHandle} {}

    void TcpSocket::bind(const uint8_t ip[4], const uint16_t port)
    {
        sockaddr localSockaddr;
        socklen_t localSockaddrLen = sizeof(localSockaddr);
        convertFromIpv4Address(ip, port, localSockaddr);

        if (0 != ::bind(mSocketHandle, &localSockaddr, localSockaddrLen))
        {
            int errorCode = errno;
            throw std::system_error(
                errorCode,
                std::system_category(),
                "Error binding socket: bind()");
        }

        if (::fcntl(mSocketHandle, F_SETFL, O_NONBLOCK) < 0) 
        {
            int errorCode = errno;
            throw std::system_error(
                errorCode,
                std::system_category(),
                "Error binding socket: fcntl()");
        }

        uint8_t localIp[4];
        std::memcpy(localIp, ip, 4);
        uint16_t localPort = port;
        if (::getsockname(mSocketHandle, &localSockaddr, &localSockaddrLen) == 0)
        {
            convertToIpv4Address(localSockaddr, localIp, localPort);
        }
    }

    void TcpSocket::listen()
    {
        if (::listen(mSocketHandle, 4096) != 0)
        {
            int errorCode = errno;
            throw std::system_error(
                errorCode,
                std::system_category(),
                "Error listening on socket: listen()");
        }
    }

    SocketAcceptTask TcpSocket::accept(TcpSocket& acceptedSocket)
    {
        return SocketAcceptTask(mIoEventHandler, *this, acceptedSocket);
    }
}