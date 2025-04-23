#include "IoEventHandler.h"
#include <system_error>
#include <cstdint>
#include <sys/epoll.h>
#include "EpollCallback.h"

namespace corolib
{
    IoEventHandler::IoEventHandler()
    {
        mEpollFd = epoll_create1(EPOLL_CLOEXEC);
    }

    void IoEventHandler::runEventLoop()
    {
        while (true)
        {
            epoll_event events[128];
            int num_fds = epoll_wait(mEpollFd, events, 128, -1);
            if (num_fds == -1)
            {
                const int errorCode = errno;
                throw std::system_error(
                    errorCode,
                    std::system_category(),
                    "Error in epoll_wait");
            }

            for (int i = 0; i < num_fds; ++i)
            {
                epoll_event& event = events[i];
                if (event.data.ptr == nullptr)
                {
                    if (event.events & EPOLLHUP)
                    {
                        const int errorCode = errno;
                        throw std::system_error(
                            errorCode,
                            std::system_category(),
                            "Error in epoll_wait EPOLLERR");
                    }
                    continue;
                }
                EpollCallback* base = reinterpret_cast<EpollCallback*>(event.data.ptr);
                base->mCallback(base, event.events);
            }
        }
    }
}
