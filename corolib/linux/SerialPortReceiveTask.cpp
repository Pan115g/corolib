#include "SerialPortReceiveTask.h"
#include <system_error>
#include <sys/epoll.h>
#include <SerialPort.h>
#include <IoEventHandler.h>

namespace corolib
{
    SerialPortReceiveTask::SerialPortReceiveTask(IoEventHandler& mIoEventHandler, SerialPort& serialPort, 
        std::span<uint8_t> buffer, const uint8_t delimiter) noexcept 
    : AwaitableIoTask<SerialPortReceiveTask>{}, 
    mIoEventHandler{mIoEventHandler},
    mSerialPort{serialPort},
    mBuffer{buffer},
    mDelimiter{delimiter} {}

    bool SerialPortReceiveTask::start()
    {
        epoll_event ev = { 0, { 0 } };
        ev.events = IoEventHandler::Default_Events;
        ev.data.ptr = this;
        if (0 != epoll_ctl(mIoEventHandler.getEpollFileDescriptor(), EPOLL_CTL_MOD, 
                mSerialPort.getFileDescriptor(), &ev))
        {
            const int errorCode = errno;
            throw std::system_error(
                errorCode,
                std::system_category(),
                "Error registering socket with epoll");
        }

        bool suspend = true;
        if (readAndFindDelimiter())
        {
            mSkipped = true;
            suspend = false;
        }
        return suspend;
    }

    SerialPortReceiveTask::ReturnType SerialPortReceiveTask::getResult()
    {
        return SerialPortReceiveTask::ReturnType{mNumberOfBytesReceived, mBufferIndex};
    }

    bool SerialPortReceiveTask::checkResumeCondition() noexcept
    {
        if (mSkipped)
        {
            return true;
        }

        if (readAndFindDelimiter())
        {
            mSkipped = true;
            return true;
        }
        return false;
    }

    bool SerialPortReceiveTask::readAndFindDelimiter()
    {
        int res = ::read(mSerialPort.getFileDescriptor(), mBuffer.data() + mNumberOfBytesReceived, 
            mBuffer.size() - mNumberOfBytesReceived);
        if (res == -1)
        {
            if (errno != EAGAIN && errno != EWOULDBLOCK)
            {
                const int errorCode = errno;
                throw std::system_error(
                    errorCode,
                    std::system_category(),
                    "Error receiving socket: recv() in start()");
            }
            return false;
        }

        if (res > 0)
        {
            mNumberOfBytesReceived += res;
            auto end = mBuffer.begin() + mNumberOfBytesReceived;
            auto iter = std::find(mBuffer.begin(), end, mDelimiter);
            if (iter != end)
            {
                mBufferIndex = std::distance(mBuffer.begin(), iter);
                mSkipped = true;
                return true;
            }
        }
        return false;
    }
}