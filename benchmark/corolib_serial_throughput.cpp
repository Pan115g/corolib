#include <benchmark/benchmark.h>
#include <future>
#include <thread>
#include <malloc.h>
#include <errno.h>
#include <cstring>
#include <iostream>
#include <termios.h>
#include "Awaitable.h"
#include "FireAndForget.h"
#include "SerialPortReceiveTask.h"
#include "SerialPortSendTask.h"
#include "IoEventHandler.h"
#include "SerialPort.h"

const std::size_t msgSize = 128;
corolib::IoEventHandler ioEventHandler;
corolib::SerialPort serialPort(ioEventHandler, "/dev/ttyACM0", 115200);

static corolib::Awaitable<> communicate(const std::span<uint8_t> message, auto onFinished)
{
    std::vector<uint8_t> buffer(message.size());
    std::copy(message.begin(), message.end(), buffer.begin());
    
    std::size_t numBytes = co_await corolib::SerialPortSendTask(ioEventHandler, serialPort, buffer);
    //auto ret = co_await corolib::SerialPortReceiveTask(ioEventHandler, serialPort, buffer, '\r');
    onFinished(numBytes);
}

static void BM_SerialSend(benchmark::State& state) {
    uint64_t finishedCounter = 0;
    std::vector<uint8_t> data(state.range(0), 'X');
    for (int i = 1; i <= state.range(0)/msgSize; i++)
    {
        data[i*msgSize - 1] = '\r'; // Ensure the last
    }

    for (auto _ : state)
    {
          std::promise<void> done;
        auto fut = done.get_future();
        corolib::fireAndForget(communicate(data, [&finishedCounter, &done](const uint64_t n){
          finishedCounter += n;
          done.set_value();
        })); 
      fut.wait();
    }
    
    std::printf("finished one run %ld %ld\n", finishedCounter, state.iterations() * state.range(0));
    state.SetComplexityN(state.range(0));
    state.SetBytesProcessed(finishedCounter);

    struct mallinfo2 mi;
    mi = mallinfo2();
    state.counters["heap allocated"] = mi.arena;  
    state.counters["heap used"] = mi.uordblks;
    state.counters["heap free"] = mi.fordblks;
}

// Register the function as a benchmark
BENCHMARK(BM_SerialSend)->Arg(msgSize * 8)->MeasureProcessCPUTime()->Complexity();



int connect_serial_port()
{
    std::thread ioThread([]() {
    try{
        ioEventHandler.runEventLoop();
      }
      catch(const std::system_error& e) {
        std::printf("Exception in IO event handler: %s\n", e.what());
      } catch (...)
      {
        std::printf("Unknown exception in IO event handler\n");
      }
    });

    ioThread.detach();
    
    return 0;
}

void flush()
{
    int fd = serialPort.getFileDescriptor();
    //tcflush(fd, TCIFLUSH);  // clear input buffer
    // tcflush(fd, TCOFLUSH); // clear output buffer
    tcflush(fd, TCIOFLUSH); // clear both
}


void receiverTask(std::stop_token stop_token, std::size_t &numOfBytes)
{
    int epfd = epoll_create1(0);
    if (epfd < 0) {
        perror("epoll_create1");
        return;
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;  // Monitor for input
    ev.data.fd = serialPort.getFileDescriptor();

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, serialPort.getFileDescriptor(), &ev) < 0) {
        perror("epoll_ctl");
        close(epfd);
        return;
    }

    struct epoll_event events[100];

    while (!stop_token.stop_requested()) {
        int nfds = epoll_wait(epfd, events, 100, 100);
        if (nfds < 0) {
            if (errno == EINTR) continue;
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == serialPort.getFileDescriptor()) {
                char buf[256];
                int n = read(serialPort.getFileDescriptor(), buf, sizeof(buf));
                if (n > 0) {
                    numOfBytes += n;
                }
            }
        }
    }
}

int main(int argc, char** argv) {
    flush();
    connect_serial_port();
    std::size_t numOfBytes = 0;
    std::jthread receiver(receiverTask, std::ref(numOfBytes));

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));  
    receiver.get_stop_source().request_stop();
    receiver.join();
    std::cout << "number of bytes received " << numOfBytes << "\n";
}
