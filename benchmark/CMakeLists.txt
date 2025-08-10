find_package (benchmark PATHS "~/tools/benchmark/build_wsl" NO_DEFAULT_PATH REQUIRED)

add_executable(corolib_socket "corolib_socket.cpp")
target_include_directories(corolib_socket PRIVATE  "../corolib")
target_include_directories(corolib_socket PRIVATE  "../corolib/linux")
target_link_libraries(corolib_socket PRIVATE  corolib benchmark::benchmark)

add_executable(corolib_serial "corolib_serial.cpp")
target_include_directories(corolib_serial PRIVATE  "../corolib")
target_include_directories(corolib_serial PRIVATE  "../corolib/linux")
target_link_libraries(corolib_serial PRIVATE  corolib benchmark::benchmark)