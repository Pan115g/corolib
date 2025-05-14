add_library (corolib "SocketAcceptTask.cpp"  "IoEventHandler.cpp" 
                    "SocketAddressConverter.cpp" "TcpSocket.cpp"
                    "SocketReceiveTask.cpp" "SocketSendTask.cpp"
                    "SerialPort.cpp" "SerialPortReceiveTask.cpp"
                    "SerialPortSendTask.cpp")
target_include_directories(corolib PRIVATE  "./")
