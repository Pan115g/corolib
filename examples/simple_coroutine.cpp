#include <iostream>
#include "Awaitable.h"
#include "SocketAcceptTask.h"
#include "IoEventHandler.h"
#include "TcpSocket.h"
#include <thread>
#include <errno.h>

corolib::Awaitable<> echo(corolib::TcpSocket& socket)
{
    try
    {
        uint8_t data[1024];
        std::span buffer{data};
        for (;;)
        {
            std::cout << "echo thread " << std::this_thread::get_id() << '\n';
            std::size_t n = co_await socket.receive(data);
            std::cout << "received bytes " << n << " in thread " << std::this_thread::get_id() << '\n';
            co_await socket.send(std::span(data, n));
        }
    }
    catch (std::exception& e)
    {
        std::printf("echo Exception: %s\n", e.what());
    }
}

corolib::Awaitable<> connect(corolib::TcpSocket& serverSocket, corolib::TcpSocket& acceptedSocket)
{
    uint8_t ip[4] = {127, 0, 0, 1};
    serverSocket.bind(ip, 55555);
    serverSocket.listen();

    co_await serverSocket.accept(acceptedSocket);
    std::cout << "Connection accepted\n";

    co_await echo(acceptedSocket);
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


