#include "SerialPort.h"
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <system_error>
#include <sys/epoll.h>
#include "IoEventHandler.h"

namespace corolib
{
    SerialPort::SerialPort(IoEventHandler& ioEventHandler, const char* deviceName, const uint32_t baudrate) 
    : mBaudrate{baudrate}, mIoEventHandler{ioEventHandler}
    {
        int s = ::open(deviceName, O_RDWR | O_NONBLOCK | O_NOCTTY);
        if (s < 0)
        {
            const int errorCode = errno;
            throw std::system_error(
                errorCode,
                std::system_category(),
                "Error opening serial port");
        }
        mFileDescriptor = s;

        setNonBlockingMode();
        setTerminalAttributes();
        registerWithEpoll();

        mInitialized = true;
    }

    SerialPort::~SerialPort()
    {
        if (mFileDescriptor != Invalid_FileDescriptor)
        {
            unregisterWithEpoll();
            ::close(mFileDescriptor);
            mFileDescriptor = Invalid_FileDescriptor;
        }
    }

    void SerialPort::setNonBlockingMode()
    {
        int fileStatus = ::fcntl(mFileDescriptor, F_GETFL);
        if (::fcntl(mFileDescriptor, F_SETFL, fileStatus | O_NONBLOCK) < 0) 
        {
            const int errorCode = errno;
            throw std::system_error(
                errorCode,
                std::system_category(),
                "Error setting non-blocking mode");
        }
    }

    void SerialPort::setTerminalAttributes()
    {
        termios ios;
        if (::tcgetattr(mFileDescriptor, &ios) < 0)
        {
            const int errorCode = errno;
            throw std::system_error(
                errorCode,
                std::system_category(),
                "Error getting terminal attributes");
        }

        speed_t baud;

        switch (mBaudrate)
        {
            case 2400:
                baud = B2400;
                break;
            case 4800:
                baud = B4800;
                break;
            case 9600:
                baud = B9600;
                break;
            case 19200:
                baud = B19200;
                break;
            case 38400:
                baud = B38400;
                break;
            case 57600:
                baud = B57600;
                break;
            case 115200:
                baud = B115200;
                break;
            case 230400:        
                baud = B230400;
                break;
            case 460800:
                baud = B460800;
                break;
            case 921600:    
                baud = B921600;
                break;
            case 1500000:
                baud = B1500000;
                break;
            case 2000000:
                baud = B2000000;
                break;
            case 3000000:
                baud = B3000000;
                break;
            case 4000000:
                baud = B4000000;
                break;
            default:
                throw std::system_error(
                    EINVAL,
                    std::system_category(),
                    "Unsupported baudrate");
        }
        ::cfsetspeed(&ios, baud);
        ::cfmakeraw(&ios);
        ios.c_iflag |= IGNPAR;
        ios.c_cflag |= CREAD | CLOCAL;
        if (::tcsetattr(mFileDescriptor, TCSANOW, &ios) < 0)
        {
            const int errorCode = errno;
            throw std::system_error(
                errorCode,
                std::system_category(),
                "Error setting terminal attributes");
        }
    }

    void SerialPort::registerWithEpoll()
    {
        epoll_event ev = { 0, { 0 } };
        ev.events = IoEventHandler::Default_Events;
        if (0 != epoll_ctl(mIoEventHandler.getEpollFileDescriptor(), EPOLL_CTL_ADD, mFileDescriptor, &ev))
        {
            const int errorCode = errno;
            throw std::system_error(
                errorCode,
                std::system_category(),
                "Error registering serial port with epoll");
        }
    }

    void SerialPort::unregisterWithEpoll()
    {
        epoll_event ev = { 0, { 0 } };
        if (0 != epoll_ctl(mIoEventHandler.getEpollFileDescriptor(), EPOLL_CTL_DEL, mFileDescriptor, &ev))
        {
            const int errorCode = errno;
            throw std::system_error(
                errorCode,
                std::system_category(),
                "Error unregistering serial port with epoll");
        }
    }
}