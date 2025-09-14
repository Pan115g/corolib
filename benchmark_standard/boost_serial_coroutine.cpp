#include <benchmark/benchmark.h>
#include <malloc.h>
#include <ctime>
#include <iostream>
#include <string>
#include <thread>
#include <span>
#include <vector>
#include <boost/asio.hpp>

using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;

boost::asio::io_context io(1);
boost::asio::serial_port serial(io, "/dev/ttyACM0");  // Change to your device

static awaitable<void> communicate(const std::span<uint8_t> message, auto onFinished)
{
    co_await async_write(serial, boost::asio::buffer(message), use_awaitable);
    std::size_t numOfBytes = co_await async_read(serial, boost::asio::buffer(message, message.size()), use_awaitable);
    onFinished(numOfBytes);   
}

static void BM_SerialSend(benchmark::State& state) {
    uint64_t finishedCounter = 0;
    std::vector<uint8_t> data(state.range(0), 'X');
    data[state.range(0) - 1] = '\r'; // Ensure the last

    for (auto _ : state)
    {
        std::promise<void> done;
        auto fut = done.get_future();
        co_spawn(io, communicate(data, [&finishedCounter, &done](const uint64_t n){
          finishedCounter += n;
          done.set_value();
        }), detached);
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
BENCHMARK(BM_SerialSend)->Arg(128)->MeasureProcessCPUTime()->Complexity();

void test_serial()
{
    boost::system::error_code ignored_error;
    int fd = serial.native_handle();
    //tcflush(fd, TCIFLUSH);  // clear input buffer
    // tcflush(fd, TCOFLUSH); // clear output buffer
    tcflush(fd, TCIOFLUSH); // clear both
    uint8_t msg[] = "hello from wsl\r";
    std::size_t len_msg = sizeof(msg) - 1;
    uint8_t rcv_msg[200] = {0};
    std::size_t n = boost::asio::write(serial, boost::asio::buffer(msg, len_msg), ignored_error);
    n = boost::asio::read(serial, boost::asio::buffer(&rcv_msg[0], len_msg), ignored_error);
    std::printf("recieved %ld %s\n", n, reinterpret_cast<char*>(&rcv_msg[0]));

}

int main(int argc, char** argv)
{
    std::cout << "main thread " << std::this_thread::get_id() << '\n';
    serial.set_option(boost::asio::serial_port_base::baud_rate(115200));
    serial.set_option(boost::asio::serial_port_base::character_size(8));
    serial.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
    serial.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
    serial.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));

    test_serial();
    try
    {
        //keep io_context alive
        boost::asio::signal_set signals(io, SIGINT, SIGTERM);
        signals.async_wait([&](auto, auto) { io.stop(); });

        std::thread ioThread([]() {
            try{
                io.run();
            }
            catch(const std::system_error& e) {
                std::printf("Exception in IO event handler: %s\n", e.what());
            } catch (...)
            {
                std::printf("Unknown exception in IO event handle\n");
            }
            });
        ioThread.detach();
        
        benchmark::Initialize(&argc, argv);
        benchmark::RunSpecifiedBenchmarks();
        benchmark::Shutdown();
    }
    catch (std::exception& e)
    {
        std::printf("Exception: %s\n", e.what());
    }
}