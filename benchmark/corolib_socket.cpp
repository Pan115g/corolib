#include <benchmark/benchmark.h>
#include <malloc.h>
#include "Awaitable.h"
#include "FireAndForget.h"
#include "SocketAcceptTask.h"
#include "IoEventHandler.h"
#include "TcpSocket.h"
#include "SocketSendTask.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <future>

using namespace corolib;
corolib::IoEventHandler ioEventHandler;
corolib::TcpSocket serverSocket(ioEventHandler);
corolib::TcpSocket acceptedSocket(ioEventHandler, corolib::TcpSocket::Invalid_Socket);

std::chrono::time_point<std::chrono::steady_clock> start_time;
std::chrono::time_point<std::chrono::steady_clock> end_time;

//template<auto onFinished>
corolib::Awaitable<void> communicate(const std::span<uint8_t> message, auto onFinished)
{  
    std::vector<uint8_t> buffer(message.size());
    std::copy(message.begin(), message.end(), buffer.begin());
    
    co_await acceptedSocket.send(buffer);
    uint32_t numOfBytes = co_await acceptedSocket.receive(buffer);
    onFinished(numOfBytes);
}

static void BM_StringCreation(benchmark::State& state) {
    uint64_t finishedCounter = 0;
    std::vector<uint8_t> data(state.range(0), 'X');
    data[state.range(0) - 1] = '\r'; // Ensure the last

    uint32_t count = 0;
    for (auto _ : state)
    {        
        data[0] = count++;
        std::promise<void> done;
        auto fut = done.get_future();
        corolib::fireAndForget(communicate(data, [&finishedCounter, &done](const uint64_t n){
          finishedCounter += n;
          done.set_value();
        }));
      //std::this_thread::sleep_for(std::chrono::milliseconds(5)); 
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
BENCHMARK(BM_StringCreation)->Arg(1024)->MeasureProcessCPUTime()->Complexity();


corolib::Awaitable<> connect(corolib::TcpSocket& serverSocket, corolib::TcpSocket& acceptedSocket)
{
    uint8_t ip[4] = {0, 0, 0, 0};
    serverSocket.bind(ip, 55555);
    serverSocket.listen();
    co_await serverSocket.accept(acceptedSocket);
    std::cout << "Connection accepted\n";
}


int connect_socket()
{
    std::printf("before connect\n");
    corolib::fireAndForget(connect(serverSocket, acceptedSocket));
  return 0;
}

int main(int argc, char** argv) {
    connect_socket();
    std::thread ioThread([]() {
      try{
        ioEventHandler.runEventLoop();
      }
      catch(const std::system_error& e) {
        std::cerr << "Exception in IO event handler: " << e.what() << std::endl;
      } catch (...)
      {
        std::cerr << "Unknown exception in IO event handler" << std::endl;
      }
    });

    ioThread.detach();
    std::this_thread::sleep_for(std::chrono::seconds(5)); // Allow some time for the connection to be established

  /*std::vector<uint8_t> data(8, 'X');
  data[7] = '\r'; // Ensure the last character is a carriage return
  uint32_t finishedCounter = 0;
  for (int i = 0; i < 100000; ++i)
  {
    corolib::fireAndForget(communicate(data, [&finishedCounter](){ finishedCounter++; }));
    //std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  std::printf("Sent message %d %d\n", finishedCounter, counter_send);
  
  std::this_thread::sleep_for(std::chrono::seconds(5));*/
  //benchmark::MaybeReenterWithoutASLR(argc, argv);
  
  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();

}
