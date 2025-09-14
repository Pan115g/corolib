#include <benchmark/benchmark.h>
#include <malloc.h>
#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
boost::asio::io_context io_context;
tcp::socket server_socket(io_context);

static void BM_StringCreation(benchmark::State& state) {
    uint64_t finishedCounter = 0;
    std::vector<uint8_t> data(state.range(0), 'X');
    data[state.range(0) - 1] = '\r'; // Ensure the last
    for (auto _ : state)
    {
        boost::system::error_code ignored_error;
        boost::asio::write(server_socket, boost::asio::buffer(data), ignored_error);
        finishedCounter += boost::asio::read(server_socket, boost::asio::buffer(&data[0], state.range(0)), ignored_error);
    }
    
    std::printf("finished one run %ld %ld\n",  finishedCounter, state.iterations() * state.range(0));
    state.SetComplexityN(state.range(0));
    state.SetBytesProcessed(finishedCounter);
    struct mallinfo2 mi;
    mi = mallinfo2();
    state.counters["heap allocated"] = mi.arena;  
    state.counters["heap used"] = mi.uordblks;
    state.counters["heap free"] = mi.fordblks;
}
// Register the function as a benchmark
BENCHMARK(BM_StringCreation)->Arg(1024)->Complexity();

int main(int argc, char** argv) {
    try
    { 
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 55555));
        
        acceptor.accept(server_socket);

        boost::asio::ip::address addr = server_socket.remote_endpoint().address();
        unsigned short port = server_socket.remote_endpoint().port();
        std::cout << "connected client " << addr.to_string() << ":" << port << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
    return 0;
}
