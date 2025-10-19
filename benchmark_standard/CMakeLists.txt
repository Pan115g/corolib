find_package (benchmark PATHS "~/tools/benchmark/build_rasp" NO_DEFAULT_PATH REQUIRED)
get_target_property(BENCHMARK_PATH benchmark::benchmark LOCATION)
message(STATUS "Google Benchmark library: ${BENCHMARK_PATH}")

#set(BOOST_ROOT "/mnt/c/Users/pan.li/source/works/boost_1_87_0")
find_package(Boost 1.88 REQUIRED COMPONENTS thread system) 

add_executable(boost_serial "boost_serial.cpp")
target_link_libraries(boost_serial PRIVATE  Boost::boost benchmark::benchmark)

add_executable(boost_serial_coroutine "boost_serial_coroutine.cpp")
target_link_libraries(boost_serial_coroutine PRIVATE  Boost::boost benchmark::benchmark)

add_executable(boost_serial_throughput "boost_serial_throughput.cpp")
target_link_libraries(boost_serial_throughput PRIVATE  Boost::boost benchmark::benchmark)

add_executable(boost_serial_coroutine_throughput "boost_serial_coroutine_throughput.cpp")
target_link_libraries(boost_serial_coroutine_throughput PRIVATE  Boost::boost benchmark::benchmark)

add_executable(boost_socket_callback "boost_socket_callback.cpp")
target_link_libraries(boost_socket_callback PRIVATE  Boost::boost benchmark::benchmark)

add_executable(boost_socket_callback_throughput "boost_socket_callback_throughput.cpp")
target_link_libraries(boost_socket_callback_throughput PRIVATE  Boost::boost benchmark::benchmark)

add_executable(boost_socket "boost_socket.cpp")
target_link_libraries(boost_socket PRIVATE  Boost::boost benchmark::benchmark)

add_executable(boost_socket_coroutine "boost_socket_coroutine.cpp")
target_link_libraries(boost_socket_coroutine PRIVATE  Boost::boost benchmark::benchmark)

add_executable(boost_socket_throughput "boost_socket_throughput.cpp")
target_link_libraries(boost_socket_throughput PRIVATE  Boost::boost benchmark::benchmark)

add_executable(boost_socket_coroutine_throughput "boost_socket_coroutine_throughput.cpp")
target_link_libraries(boost_socket_coroutine_throughput PRIVATE  Boost::boost benchmark::benchmark)
