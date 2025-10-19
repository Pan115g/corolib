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

    SerialPortSendTask::~SerialPortSendTask()
    {
        if (mSerialPort.isInitialized())
        {
            epoll_event ev = { 0, { 0 } };
            ev.events = IoEventHandler::Default_Events & ~EPOLLOUT;
            if (0 != epoll_ctl(mIoEventHandler.getEpollFileDescriptor(), EPOLL_CTL_MOD, 
                                mSerialPort.getFileDescriptor(), &ev))
            {
                mNumberOfBytesSent = 0;
            }
        }
    }

    bool SerialPortSendTask::start()
    {        
        if (sendAndCheckNumberOfSentBytes())
        {
            mSkipped = true;
            return false;
        }

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

        return true;
    }

    bool SerialPortSendTask::checkResumeCondition(uint32_t events) noexcept
    {
        if ((events & EPOLLOUT) == 0)
        {
            return false;
        }

        if (mSkipped)
        {
            return true;
        }

        mSkipped = sendAndCheckNumberOfSentBytes();
        return mSkipped;
    }

    std::size_t SerialPortSendTask::getResult()
    {
        return mNumberOfBytesSent;
    }

    bool SerialPortSendTask::sendAndCheckNumberOfSentBytes()
    {
        int res = ::write(mSerialPort.getFileDescriptor(), mBuffer.data() + mNumberOfBytesSent, mBuffer.size() - mNumberOfBytesSent);
        if (res == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
        {
            const int errorCode = errno;
            throw std::system_error(
                errorCode,
                std::system_category(),
                "Error sending serial port: send() in checkResumeCondition()");
        }

        if (res > 0)
        {
            mNumberOfBytesSent += res;
        }
        return mNumberOfBytesSent == mBuffer.size();
    }
}