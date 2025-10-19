find_package (benchmark PATHS "~/tools/benchmark/build_rasp" NO_DEFAULT_PATH REQUIRED)


add_executable(corolib_socket "corolib_socket.cpp")
target_include_directories(corolib_socket PRIVATE  "../corolib")
target_include_directories(corolib_socket PRIVATE  "../corolib/linux")
target_link_libraries(corolib_socket PRIVATE  corolib benchmark::benchmark)

add_executable(corolib_serial "corolib_serial.cpp")
target_include_directories(corolib_serial PRIVATE  "../corolib")
target_include_directories(corolib_serial PRIVATE  "../corolib/linux")
target_link_libraries(corolib_serial PRIVATE  corolib benchmark::benchmark)


add_executable(corolib_socket_throughput "corolib_socket_throughput.cpp")
target_include_directories(corolib_socket_throughput PRIVATE  "../corolib")
target_include_directories(corolib_socket_throughput PRIVATE  "../corolib/linux")
target_link_libraries(corolib_socket_throughput PRIVATE  corolib benchmark::benchmark)

add_executable(corolib_serial_throughput "corolib_serial_throughput.cpp")
target_include_directories(corolib_serial_throughput PRIVATE  "../corolib")
target_include_directories(corolib_serial_throughput PRIVATE  "../corolib/linux")
target_link_libraries(corolib_serial_throughput PRIVATE  corolib benchmark::benchmark)