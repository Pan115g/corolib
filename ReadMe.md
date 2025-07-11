# Introduction
The c++ library supports writing asynchronous code for I/O communication with coroutine.
Supporte platforms:
 - windows
 - linux
 - FreeRTOS

The source code of the library is in folder corolib.  
Example projects are
 - examples (cmake project for linux)
 - STM32CubeIDE (STM32Cube project for master, uses FreeRTOS)
 - MCUSlave (STM32Cube project for slave, bare metal) 


# Configure serial port for WSL
This configuration is necessary if you don't have native linux OS, but want to configure the ST-Link debug Uart port for windows WSL.
## Steps
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
 
# Setup working repository
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
