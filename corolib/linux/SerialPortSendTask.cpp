#include "SerialPortSendTask.h"
#include <system_error>
#include <sys/epoll.h>
#include <SerialPort.h>
#include <IoEventHandler.h>

namespace corolib
{
    SerialPortSendTask::SerialPortSendTask(IoEventHandler& mIoEventHandler, SerialPort& serialPort, const std::span<uint8_t> buffer) noexcept 
    : AwaitableIoTask<SerialPortSendTask>{}, 
    mIoEventHandler{mIoEventHandler},
    mSerialPort{serialPort},
    mBuffer{buffer}
    {
    }

    bool SerialPortSendTask::start()
    {
        epoll_event ev = { 0, { 0 } };
        ev.events = IoEventHandler::Default_Events | EPOLLOUT;
        ev.data.ptr = this;
        if (0 != epoll_ctl(mIoEventHandler.getEpollFileDescriptor(), EPOLL_CTL_MOD, 
            mSerialPort.getFileDescriptor(), &ev))
        {
            const int errorCode = errno;
            throw std::system_error(
                errorCode,
                std::system_category(),
                "Error registering serial port with epoll");
        }

        int res = ::write(mSerialPort.getFileDescriptor(), mBuffer.data(), mBuffer.size());
        if (res > 0)
        {
            mSkipped = true;
            mNumberOfBytesSent = res;
            return false;
        }

        if (res == -1)
        {
            if (errno != EAGAIN && errno != EWOULDBLOCK)
            {
                const int errorCode = errno;
                throw std::system_error(
                    errorCode,
                    std::system_category(),
                    "Error receiving serial port: send() in start()");
            }
        }
        return true;
    }

    std::size_t SerialPortSendTask::getResult()
    {
        if (mSkipped)
        {
            return mNumberOfBytesSent;
        }

        int res = ::write(mSerialPort.getFileDescriptor(), mBuffer.data(), mBuffer.size());
        if (res == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
        {
            const int errorCode = errno;
            throw std::system_error(
                errorCode,
                std::system_category(),
                "Error receiving serial port: send() in getResult()");
        }

        if (res > 0)
        {
            mNumberOfBytesSent = res;
        }
        return mNumberOfBytesSent;
    }
}