find_package (benchmark PATHS "~/tools/benchmark/build_wsl" NO_DEFAULT_PATH REQUIRED)

add_executable(benchmark_app "benchmark_app.cpp")
target_include_directories(benchmark_app PRIVATE  "../corolib")
target_include_directories(benchmark_app PRIVATE  "../corolib/linux")
target_link_libraries(benchmark_app PRIVATE  corolib benchmark::benchmark)