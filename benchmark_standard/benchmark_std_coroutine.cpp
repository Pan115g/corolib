#include <benchmark/benchmark.h>
#include <malloc.h>
#include <ctime>
#include <iostream>
#include <string>
#include <thread>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/awaitable.hpp>
#include <cstdio>
#include <iostream>
#include <span>

using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
namespace this_coro = boost::asio::this_coro;


#if defined(BOOST_ASIO_ENABLE_HANDLER_TRACKING)
# define use_awaitable \
  boost::asio::use_awaitable_t(__FILE__, __LINE__, __PRETTY_FUNCTION__)
#endif

boost::asio::io_context io_context(1);
tcp::socket server_socket(io_context);

awaitable<void> communicate(const std::span<uint8_t> message, auto onFinished)
{
    std::size_t numOfBytes = co_await async_write(server_socket, boost::asio::buffer(message), use_awaitable);
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
        co_spawn(io_context, communicate(data, [&finishedCounter](const uint64_t n){
          finishedCounter += n;
        }), detached);
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
BENCHMARK(BM_StringCreation)->Arg(1024)->MeasureProcessCPUTime()->Complexity();

awaitable<void> listener()
{
    auto executor = co_await this_coro::executor;
    tcp::acceptor acceptor(executor, { tcp::v4(), 55555 });
    std::cout << "listener thread " << std::this_thread::get_id() << '\n';
    tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
    boost::asio::ip::address addr = socket.remote_endpoint().address();
    unsigned short port = socket.remote_endpoint().port();
    std::printf("connected client %s %d\n", addr.to_string().c_str(), port);

    server_socket = std::move(socket);
    
}

int main(int argc, char** argv)
{
    std::cout << "main thread " << std::this_thread::get_id() << '\n';
    try
    {
        

        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait([&](auto, auto) { io_context.stop(); });

        co_spawn(io_context, listener(), detached);

        std::thread ioThread([]() {
            try{
                io_context.run();
            }
            catch(const std::system_error& e) {
                std::cerr << "Exception in IO event handler: " << e.what() << std::endl;
            } catch (...)
            {
                std::cerr << "Unknown exception in IO event handler" << std::endl;
            }
            });
        ioThread.detach();

        std::this_thread::sleep_for(std::chrono::seconds(5));
        
        benchmark::Initialize(&argc, argv);
        benchmark::RunSpecifiedBenchmarks();
        benchmark::Shutdown();
    }
    catch (std::exception& e)
    {
        std::printf("Exception: %s\n", e.what());
    }
}