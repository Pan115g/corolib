#include <benchmark/benchmark.h>
#include <malloc.h>
#include <thread>
#include <boost/asio.hpp>

boost::asio::io_context io;
boost::asio::serial_port serial(io, "/tmp/ttyV1");  // Change to your device

static void BM_SerialSend(benchmark::State& state) {
    uint64_t finishedCounter = 0;
    std::vector<uint8_t> data(state.range(0), 'X');
    data[state.range(0) - 1] = '\r'; // Ensure the last
    for (auto _ : state)
    {
        boost::system::error_code ignored_error;
        finishedCounter += boost::asio::write(serial, boost::asio::buffer(data), ignored_error);
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
BENCHMARK(BM_SerialSend)->Arg(1024)->Complexity();

int main(int argc, char** argv) {
    try {
        // Set serial port options
        serial.set_option(boost::asio::serial_port_base::baud_rate(115200));
        serial.set_option(boost::asio::serial_port_base::character_size(8));
        serial.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
        serial.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
        serial.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));

    } catch (const std::exception& e) {
        std::printf("Exception: %s\n", e.what());
    }

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
    return 0;
}