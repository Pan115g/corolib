add_executable(main_app "main.cpp" "test_socket.cpp" "test_serial_port.cpp")
target_include_directories(main_app PRIVATE  "../corolib")
target_include_directories(main_app PRIVATE  "../corolib/linux")
target_link_libraries(main_app PRIVATE  corolib)