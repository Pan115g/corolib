#include <benchmark/benchmark.h>
#include <thread>
#include <malloc.h>
#include <errno.h>
#include <cstring>
#include "Awaitable.h"
#include "FireAndForget.h"
#include "SerialPortReceiveTask.h"
#include "SerialPortSendTask.h"
#include "IoEventHandler.h"
#include "SerialPort.h"

corolib::IoEventHandler ioEventHandler;
corolib::SerialPort serialPort(ioEventHandler, "/tmp/ttyV1", 115200);

static corolib::Awaitable<> communicate(const std::span<uint8_t> message, auto onFinished)
{
    std::vector<uint8_t> buffer(message.size());
    std::copy(message.begin(), message.end(), buffer.begin());
    
    uint32_t numOfBytes = co_await corolib::SerialPortSendTask(ioEventHandler, serialPort, buffer);
    onFinished(numOfBytes);
}

static void BM_SerialSend(benchmark::State& state) {
    uint64_t finishedCounter = 0;
    std::vector<uint8_t> data(state.range(0), 'X');

    data[state.range(0) - 1] = '\r'; // Ensure the last

    for (auto _ : state)
    {
        corolib::fireAndForget(communicate(data, [&finishedCounter](const uint64_t n){
          finishedCounter += n;
        }));
      std::this_thread::sleep_for(std::chrono::milliseconds(5));  
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
BENCHMARK(BM_SerialSend)->Arg(32)->MeasureProcessCPUTime()->Complexity();



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


int main(int argc, char** argv) {
    connect_serial_port();

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
}
