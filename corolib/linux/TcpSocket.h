#ifndef TCPSOCKET_H_    
#define TCPSOCKET_H_

#include <cstdint>
#include "IoEventHandler.h"
#include "SocketAcceptTask.h"
#include "SocketSendTask.h"
#include "SocketReceiveTask.h"

namespace corolib
{
    class TcpSocket
    {
        public:

        constexpr static int32_t Invalid_Socket = -1;

        TcpSocket();
        explicit TcpSocket(IoEventHandler& ioEventHandler);
        explicit TcpSocket(IoEventHandler& ioEventHandler, int socketHandle);
        TcpSocket(const TcpSocket&) = delete;

        ~TcpSocket();

        int getSocketHandle() const { return mSocketHandle; }
        void setSocketHandle(int socketHandle);
        bool isInitialized() const { return mInitialized; }
        
        uint8_t* getLocalIp() { return localIp; }
        uint16_t& getLocalPort() { return localPort; }
        
        uint8_t* getRemoteIp() { return remoteIp; }
        uint16_t& getRemotePort() { return remotePort; }

        void bind(const uint8_t ip[4], const uint16_t port);    
        void listen();

        SocketAcceptTask accept(TcpSocket& acceptedSocket)
        {
            return SocketAcceptTask(mIoEventHandler, *this, acceptedSocket);
        }

        SocketSendTask send(const std::span<uint8_t> buffer)
        {
            return SocketSendTask(mIoEventHandler, *this, buffer);
        }

        SocketReceiveTask receive(std::span<uint8_t> buffer)        
        {
            return SocketReceiveTask(mIoEventHandler, *this, buffer);
        }

        private:
        void setNonBlockingMode();
        void registerWithEpoll();
        void unregisterWithEpoll();

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