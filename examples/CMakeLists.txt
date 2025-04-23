add_executable(simple_coroutine "simple_coroutine.cpp" )
target_include_directories(simple_coroutine PRIVATE  "../corolib")
target_include_directories(simple_coroutine PRIVATE  "../corolib/linux")
target_link_libraries(simple_coroutine PRIVATE  corolib)