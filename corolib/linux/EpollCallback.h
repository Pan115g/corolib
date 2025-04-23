#ifndef EPOLLCALLBACK_H
#define EPOLLCALLBACK_H

#include <cstdint>

namespace corolib
{    
    struct EpollCallback
    {
        using Callback = void(EpollCallback*, uint32_t);
        explicit EpollCallback(Callback* callback) : mCallback(callback) {}
        Callback* mCallback;
    };
}

#endif