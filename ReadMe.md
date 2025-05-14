# Configure serial port for WSL
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
## WSL
```
git clone /mnt/c/path_to_remote_repository
```
### git annoying warning about LF/CRLF
```
 git config --global core.autocrlf true
```
## Windows
### git annoying warning about LF/CRLF
```
 git config --global core.autocrlf false
```
