#include <iostream>
#include <thread>
#include <errno.h>
#include <cstring>
#include "Awaitable.h"
#include "FireAndForget.h"
#include "SerialPortReceiveTask.h"
#include "SerialPortSendTask.h"
#include "IoEventHandler.h"
#include "SerialPort.h"

corolib::Awaitable<> echo(corolib::IoEventHandler& ioEventHandler, corolib::SerialPort& serialPort)
{
    const uint32_t cSizeOfSendData = 11;
    const char* sendData[cSizeOfSendData] = {"Hello from serial port\r", 
        "Coroutines allocate a coroutine frame on the heap or stack to store local variables and state across suspension points.\r",
        "In embedded systems, heap usage should be minimized or avoided due to limited memory.\r",
        "Use a custom allocator or ensure that the coroutine frame size is small. Avoid large local variables or arrays in coroutines.\r", 
        "Coroutines are cooperative, meaning they yield control explicitly (e.g., at co_await points).\r",
        "This is suitable for many embedded systems but may not meet hard real-time constraints.\r",
        "Ensure that coroutines do not block for long periods or perform heavy computations.\r",
        "Integrate coroutine in eventloop is a common pattern in embedded systems.\r",
        "Ensure the event loop is efficient and does not introduce significant latency.\r",  
        "Yourcoroutine handles serial communication using SerialPortReceiveTask and SerialPortSendTask.\r", 
        "Ensure these tasks are non-blocking and use hardware interrupts or DMA for efficiency.\r"};
    uint8_t data[1024];
    std::span receiveBuffer{data};
    uint32_t receivedBytes{0};
    corolib::SerialPortReceiveTask::ReturnType received = co_await corolib::SerialPortReceiveTask(ioEventHandler, serialPort, receiveBuffer, '\r');
    std::cout << "received bytes " << received.bytesReceived << " delimiter at " << received.delimiterIndex << " in thread " << std::this_thread::get_id() << '\n';
    if (received.delimiterIndex == 0)
    {
        std::printf("The first message shouldn't be empty\n");
        co_return;
    }
    receivedBytes += received.bytesReceived;
    if (received.delimiterIndex < (receivedBytes - 1))
    {
        std::rotate(receiveBuffer.begin(), receiveBuffer.begin() + received.delimiterIndex + 1, receiveBuffer.begin() + receivedBytes);
    }
    receivedBytes -= received.delimiterIndex + 1;
    
    for (uint32_t i = 0;; i++)
    {
        std::cout << "echo thread " << std::this_thread::get_id() << '\n';
        std::size_t sentBytes = co_await corolib::SerialPortSendTask(ioEventHandler, serialPort, std::span((uint8_t*)sendData[i%cSizeOfSendData], strlen(sendData[i%cSizeOfSendData])));
        std::cout << "sent bytes " << sentBytes << " in thread " << std::this_thread::get_id() << '\n';
        received = co_await corolib::SerialPortReceiveTask(ioEventHandler, serialPort, std::span(receiveBuffer.begin()+receivedBytes, receiveBuffer.end()), '\r');
        std::cout << "received bytes " << received.bytesReceived << "delimiter at " << received.delimiterIndex << " in thread " << std::this_thread::get_id() << '\n';
        if (received.delimiterIndex == 0)
        {
            std::printf("Empty message received\n");
            co_return;
        }
        received.bytesReceived += receivedBytes;
        received.delimiterIndex += receivedBytes;
        receivedBytes += received.bytesReceived;
        if (sentBytes != received.delimiterIndex + 1)
        {
            std::printf("sent bytes %ld != received delimiter index %d\n", sentBytes, received.delimiterIndex + 1);
            co_return;
        }
        if (received.delimiterIndex < (receivedBytes - 1))
        {
            std::rotate(receiveBuffer.begin(), receiveBuffer.begin() + received.delimiterIndex + 1, receiveBuffer.begin() + receivedBytes);
        }
        receivedBytes -= received.delimiterIndex + 1;         
    }
}

int test_serial_port()
{
    corolib::IoEventHandler ioEventHandler;
    corolib::SerialPort serialPort(ioEventHandler, "/dev/ttyACM0", 115200);
    corolib::fireAndForget(echo(ioEventHandler, serialPort));
    
    ioEventHandler.runEventLoop();
    return 0;
}