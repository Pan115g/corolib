#ifndef TCPSOCKET_H_    
#define TCPSOCKET_H_

#include <cstdint>
#include <sys/epoll.h>
#include "IoEventHandler.h"
#include "SocketAcceptTask.h"
// #include "SocketSendTask.h"
// #include "SocketReceiveTask.h"

namespace corolib
{
    class TcpSocket
    {
        public:

        constexpr static int32_t Invalid_Socket = -1;
        constexpr static uint32_t Default_Events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLPRI | EPOLLET;

        TcpSocket();
        explicit TcpSocket(IoEventHandler& ioEventHandler);
        explicit TcpSocket(IoEventHandler& ioEventHandler, int socketHandle);
        TcpSocket(const TcpSocket&) = delete;

        int getSocketHandle() const { return mSocketHandle; }
        void setSocketHandle(int socketHandle) { mSocketHandle = socketHandle; }
        bool isInitialized() const { return mInitialized; }
        
        uint8_t* getLocalIp() { return localIp; }
        uint16_t& getLocalPort() { return localPort; }
        
        uint8_t* getRemoteIp() { return remoteIp; }
        uint16_t& getRemotePort() { return remotePort; }

        void bind(const uint8_t ip[4], const uint16_t port);    
        void listen();

        SocketAcceptTask accept(TcpSocket& acceptedSocket);

        // SocketSendTask send(const uint8_t* buffer, std::size_t size)
        // {
        //     return SocketSendTask(*this, const_cast<uint8_t*>(buffer), size);
        // }
        // SocketReceiveTask receive(std::uint8_t* buffer, std::size_t size)        
        // {
        //     return SocketReceiveTask(*this, buffer, size);
        // }

        private:
        bool mInitialized{false};
        int mSocketHandle;
        uint8_t localIp[4];
        uint16_t localPort;
        uint8_t remoteIp[4];
        uint16_t remotePort;
        IoEventHandler& mIoEventHandler;
    };
}

#endif