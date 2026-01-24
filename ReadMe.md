# Introduction
The C++ library supports writing asynchronous code for I/O communication with coroutine.
Supported platforms:
 - linux
 - STM32 with FreeRTOS

# Folder Structure
## The library
The source code of the library is in folder corolib. Files under corolib are platform independent. Everything which belongs to linux are under folder linux. 
For microcontrollers, I implemented the library only for its platform. For detail, see the microcontroller project folder.

## Lang time test
Communications are between Raspberry Pi5, STM32f4 and STM32f7.
 - Uart between Pi and STM32f4
 - I²C between STM32f4 and STM32f7
 - SPI between STM32f4 and STM32f7 
Sporadic request goes from Pi to STM32f4 then to STM32f7. Response returns back from STM32f7 over STM32f4 to Pi. Periodic data is exchanged between STM32f4 and STM32f7 every 10ms through SPI.

### Development environment
STM32CubeIDE Version 1.18.1.

### Master
Folder STM32CubeIDE contains the FreeRTOS STM32Cube project for master.

### Slave
Folder MCUSlave contains bare metal STM32Cube project for slave.

## Benchmark test
The benchmark test was implemented for Socket and Uart. boost asio test routines are taken as standard. My aim was not to compare with boost library, but to compare the performance of C++ Coroutine based communication with the Callback based communication.
Folder benchmark_standard contains the boost routines.
Folder benchmark contains the routines implemented with this library.

### Through put test
 - boost_xx_throughput.cpp: synchronous variant
 - boost_xx_coroutine_throughput.cpp: coroutine variant
 - boost_xx_callback_throughput.cpp: callback variant
 - corolib_xx_throughput.cpp: coroutine variant
   
### Round trip test
 - boost_xx.cpp: synchronous variant
 - boost_xx_coroutine.cpp: coroutine variant
 - boost_xx_callback.cpp: callback variant
 - coroblib_xx: coroutine variant

## Example project
Folder examples contains a cmake project for linux.


# FAQ
## How to configure serial port for WSL
This configuration is necessary if you don't have native linux OS, but want to configure the ST-Link debug Uart port for windows WSL.
### Steps
 - install usbipd-win_5.0.0_x64.msi
 - open powershell
 - list all usb devices with
```
 usbipd list
 Connected:
BUSID  VID:PID    DEVICE                                                        STATE
2-2    045e:0c1e  Surface Camera Front, Surface IR Camera Front                 Not shared
2-10   8087:0029  Intel(R) Wireless Bluetooth(R)                                Not shared
5-3    046d:c52f  USB-Eingabegerät                                              Not shared
6-4    0483:374b  ST-Link Debug, USB-Massenspeichergerät, STMicroelectronic...  Shared
```
 - attach the usb device to WSL with
 ```
 usbipd attach --wsl --busid 6-4
 ```
 - if successful, the device file is visible in WSL
 ```
 ls /dev/ttyACM0 
 ```
 - to detach the serial port use
 ```
 usbipd detach --busid 6-4
 ```
 
## How to solve LF/CRLF problems between linux and windows
## In WSL
Clone local repository which is located in windows.
```
git clone /mnt/c/path_to_remote_repository
```
### git annoying warning about LF/CRLF
```
 git config --global core.autocrlf true
```
## In Windows
### git annoying warning about LF/CRLF
```
 git config --global core.autocrlf false
```

# Publications
* **Coroutine-basierte I/O-Kommunikation in eingebetteten Systemen**, Pan Li, *Tagungsband Embedded Software Engineering Kongress 2025*, 2025 [Link](https://ese-kongress.de/frontend/index.php?page_id=45441)
