#include <benchmark/benchmark.h>
#include <malloc.h>
#include <thread>
#include <boost/asio.hpp>

boost::asio::io_context io;
boost::asio::serial_port serial(io, "/dev/ttyACM0");  // Change to your device

static void BM_SerialSend(benchmark::State& state) {
    uint64_t finishedCounter = 0;
    std::vector<uint8_t> data(state.range(0), 'X');
    data[state.range(0) - 1] = '\r'; // Ensure the last
    for (auto _ : state)
    {
        boost::system::error_code ignored_error;
        boost::asio::write(serial, boost::asio::buffer(data, state.range(0)), ignored_error);
        finishedCounter += boost::asio::read(serial, boost::asio::buffer(&data[0], state.range(0)), ignored_error);
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
BENCHMARK(BM_SerialSend)->Arg(128)->Complexity();

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

int main(int argc, char** argv) {
    try {
        // Set serial port options
        serial.set_option(boost::asio::serial_port_base::baud_rate(115200));
        serial.set_option(boost::asio::serial_port_base::character_size(8));
        serial.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
        serial.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
        serial.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));

        test_serial();

    } catch (const std::exception& e) {
        std::printf("Exception: %s\n", e.what());
    }

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
    return 0;
}