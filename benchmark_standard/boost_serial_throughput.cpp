#include <benchmark/benchmark.h>
#include <malloc.h>
#include <thread>
#include <iostream>
#include <boost/asio.hpp>

const std::size_t msgSize = 128;
boost::asio::io_context io;
boost::asio::serial_port serial(io, "/dev/ttyACM0");  // Change to your device

static void BM_SerialSend(benchmark::State& state) {
    uint64_t finishedCounter = 0;
    std::vector<uint8_t> data(state.range(0), 'X');
    for (int i = 1; i <= state.range(0)/msgSize; i++)
    {
        data[i*msgSize - 1] = '\r'; // Ensure the last
    }

    for (auto _ : state)
    {
        boost::system::error_code ignored_error;
        finishedCounter += boost::asio::write(serial, boost::asio::buffer(data, state.range(0)), ignored_error);
        //finishedCounter += boost::asio::read(serial, boost::asio::buffer(&data[0], state.range(0)), ignored_error);
        //std::this_thread::sleep_for(std::chrono::milliseconds(20)); 
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
BENCHMARK(BM_SerialSend)->Arg(msgSize * 8)->MeasureProcessCPUTime()->Complexity();

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


void receiverTask(std::stop_token stop_token, std::size_t &numOfBytes)
{
    int epfd = epoll_create1(0);
    if (epfd < 0) {
        perror("epoll_create1");
        return;
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;  // Monitor for input
    ev.data.fd = serial.native_handle();

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, serial.native_handle(), &ev) < 0) {
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
            if (events[i].data.fd == serial.native_handle()) {
                char buf[256];
                int n = read(serial.native_handle(), buf, sizeof(buf));
                if (n > 0) {
                    numOfBytes += n;
                }
            }
        }
    }
}

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

    test_serial();
    std::size_t numOfBytes = 0;
    std::jthread receiver(receiverTask, std::ref(numOfBytes));

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));  
    receiver.get_stop_source().request_stop();
    receiver.join();
    std::cout << "number of bytes received " << numOfBytes << "\n";
    return 0;
}