#ifndef FIREANDFORGET_H
#define FIREANDFORGET_H

#include "EntryPointAwaitable.h"

namespace corolib {
    
    template <typename AWAITABLE>
    EntryPointAwaitable entryPoint(AWAITABLE t) {
        co_await std::move(t);
    }

    template <typename AWAITABLE>
    void fireAndForget(AWAITABLE t) 
    {
        entryPoint(std::move(t)).resume();
    }
}

#endif