#ifndef IOEVENTHANDLER_H_    
#define IOEVENTHANDLER_H_

#include <unistd.h>
#include <cstdint>
#include <sys/epoll.h>

namespace corolib
{
    class IoEventHandler
    {
        public:
        
        constexpr static uint32_t Default_Events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLPRI | EPOLLET;

        IoEventHandler();
        IoEventHandler(const IoEventHandler&) = delete;
        IoEventHandler(IoEventHandler&& other) noexcept : mEpollFd(other.mEpollFd)
        {
            other.mEpollFd = -1;
        }

        ~IoEventHandler()
        {
            if (mEpollFd != -1)
            {
                ::close(mEpollFd);
                mEpollFd = -1;
            }
        }
        IoEventHandler& operator=(const IoEventHandler&) = delete;

        IoEventHandler& operator=(IoEventHandler&& other) noexcept
        {
            if (mEpollFd != -1 && mEpollFd != other.mEpollFd)
            {
                ::close(mEpollFd);
            }
            mEpollFd = other.mEpollFd;
            other.mEpollFd = -1;
            
            return *this;
        }

        int getEpollFileDescriptor() const { return mEpollFd; }

        void runEventLoop();

        private:
        int mEpollFd;
    };
}

#endif