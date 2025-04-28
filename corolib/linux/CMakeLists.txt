add_library (corolib "SocketAcceptTask.cpp"  "IoEventHandler.cpp" 
                    "SocketAddressConverter.cpp" "TcpSocket.cpp"
                    "SocketReceiveTask.cpp" "SocketSendTask.cpp")
target_include_directories(corolib PRIVATE  "./")
