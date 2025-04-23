#include <iostream>
#include "Awaitable.h"
#include "SocketAcceptTask.h"
#include "IoEventHandler.h"
#include "TcpSocket.h"
#include <thread>
#include <errno.h>

corolib::Awaitable<> connect(corolib::TcpSocket& serverSocket, corolib::TcpSocket& acceptedSocket)
{
    uint8_t ip[4] = {127, 0, 0, 1};
    serverSocket.bind(ip, 55555);
    serverSocket.listen();

    co_await serverSocket.accept(acceptedSocket);
    std::cout << "Connection accepted\n";
}


int main()
{
    corolib::IoEventHandler ioEventHandler;
    corolib::TcpSocket serverSocket(ioEventHandler);
    corolib::TcpSocket acceptedSocket(ioEventHandler, corolib::TcpSocket::Invalid_Socket);
    auto task = connect(serverSocket, acceptedSocket);
    task.resume();
    ioEventHandler.runEventLoop();
    return 0;
}


