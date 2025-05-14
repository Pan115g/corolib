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
            std::size_t recvBytes = co_await socket.receive(data);
            std::cout << "received bytes " << recvBytes << " in thread " << std::this_thread::get_id() << '\n';
            std::size_t sentBytes = co_await socket.send(std::span(data, recvBytes));
            std::cout << "sent bytes " << sentBytes << " in thread " << std::this_thread::get_id() << '\n';
            if (sentBytes != recvBytes)
            {                
                break;
            }
        }
    }
    catch (std::exception& e)
    {
        std::printf("echo Exception: %s\n", e.what());
    }
}

corolib::Awaitable<> connect(corolib::TcpSocket& serverSocket, corolib::TcpSocket& acceptedSocket)
{
    uint8_t ip[4] = {192, 168, 178, 52};
    serverSocket.bind(ip, 55555);
    serverSocket.listen();

    co_await serverSocket.accept(acceptedSocket);
    std::cout << "Connection accepted\n";

    co_await echo(acceptedSocket);
}


int test_socket()
{
    corolib::IoEventHandler ioEventHandler;
    corolib::TcpSocket serverSocket(ioEventHandler);
    corolib::TcpSocket acceptedSocket(ioEventHandler, corolib::TcpSocket::Invalid_Socket);
    auto task = connect(serverSocket, acceptedSocket);
    task.resume();
    ioEventHandler.runEventLoop();
    return 0;
}


