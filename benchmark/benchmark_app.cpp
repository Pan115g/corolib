#include <benchmark/benchmark.h>
#include <malloc.h>
#include <iostream>
#include "Awaitable.h"
#include "FireAndForget.h"
#include "SocketAcceptTask.h"
#include "IoEventHandler.h"
#include "TcpSocket.h"
#include "SocketSendTask.h"
#include <thread>

corolib::IoEventHandler ioEventHandler;
corolib::TcpSocket serverSocket(ioEventHandler);
corolib::TcpSocket acceptedSocket(ioEventHandler, corolib::TcpSocket::Invalid_Socket);

//template<auto onFinished>
corolib::Awaitable<void> communicate(const std::span<uint8_t> message, auto onFinished)
{
    uint8_t buffer[1024];
    std::copy(message.begin(), message.end(), buffer);
    //std::cout << "Sending " << message << " in " << std::this_thread::get_id() << std::endl; 
    uint32_t numOfBytes = co_await acceptedSocket.send({&buffer[0], message.size()});
    onFinished(numOfBytes);
}

static void BM_StringCreation(benchmark::State& state) {
    uint64_t finishedCounter = 0;
    std::vector<uint8_t> data(state.range(0), 'X');
    uint8_t i = 0;
    for(auto & c : data) {
        c = ++i + 30;
    }
    data[state.range(0) - 1] = '\r'; // Ensure the last
    for (auto _ : state)
    {
        corolib::fireAndForget(communicate(data, [&finishedCounter](const uint64_t n){
          finishedCounter += n;
        }));
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::printf("finished one run %ld %ld\n", finishedCounter,  state.iterations() * state.range(0));
    state.SetComplexityN(state.range(0));
    state.SetBytesProcessed(state.iterations() * state.range(0));
    struct mallinfo2 mi;
    mi = mallinfo2();
    state.counters["heap allocated"] = mi.arena;  
    state.counters["heap used"] = mi.uordblks;
    state.counters["heap free"] = mi.fordblks;
}
// Register the function as a benchmark
BENCHMARK(BM_StringCreation)->Arg(32)->Complexity();


corolib::Awaitable<> connect(corolib::TcpSocket& serverSocket, corolib::TcpSocket& acceptedSocket)
{
    std::printf("in connect\n");
    uint8_t ip[4] = {172, 17, 231, 208};
    serverSocket.bind(ip, 55555);
    serverSocket.listen();
    std::printf("client received\n");
    co_await serverSocket.accept(acceptedSocket);
    std::cout << "Connection accepted\n";
}


int connect_socket()
{
    std::printf("before connect\n");
    corolib::fireAndForget(connect(serverSocket, acceptedSocket));
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
}

int main(int argc, char** argv) {
  connect_socket();
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
