find_package (benchmark PATHS "~/tools/benchmark/build_wsl" NO_DEFAULT_PATH REQUIRED)
set(BOOST_ROOT "/mnt/c/Users/pan.li/source/works/boost_1_87_0")
find_package(Boost 1.88 REQUIRED COMPONENTS thread system) 

add_executable(benchmark_std "benchmark_std.cpp")
target_link_libraries(benchmark_std PRIVATE  Boost::boost benchmark::benchmark)

add_executable(benchmark_std_coroutine "benchmark_std_coroutine.cpp")
target_link_libraries(benchmark_std_coroutine PRIVATE  Boost::boost benchmark::benchmark)