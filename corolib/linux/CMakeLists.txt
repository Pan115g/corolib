add_library (corolib "SocketAcceptTask.cpp"  "IoEventHandler.cpp" 
                    "SocketAddressConverter.cpp" "TcpSocket.cpp")
target_include_directories(corolib PRIVATE  "./")
