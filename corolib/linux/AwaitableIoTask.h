#ifndef AWAITABLETASK_H_    
#define AWAITABLETASK_H_

#include "EpollCallback.h"
#include <coroutine>

namespace corolib
{
    /*
    * AwaitableTask is a base class for all awaitable tasks, such as I/O operations.
    T is the derived class that implements the actual task.
    */
    template <typename T>
    class AwaitableIoTask : public EpollCallback
    {
    public:
        explicit AwaitableIoTask() noexcept : EpollCallback{&AwaitableIoTask<T>::onTaskCompleted},
        mAwaitingCoroutine{}, mEvents{0} {}

        bool await_ready() const noexcept { return false; }

        bool await_suspend(std::coroutine_handle<> awaitingCoroutine)
        {
            mAwaitingCoroutine = awaitingCoroutine;
            return static_cast<T&>(*this).start();
        }

        uint32_t getResult() const noexcept
        {
            return mEvents;
        }

        decltype(auto) await_resume()
        {
            return static_cast<T&>(*this).getResult();
        }

    private:

        static void onTaskCompleted(
            EpollCallback* task,
            uint32_t events) noexcept
        {
            auto awaitableTask = static_cast<AwaitableIoTask<T>*>(task);
            awaitableTask->mEvents = events;
            awaitableTask->mAwaitingCoroutine.resume();
        }

        std::coroutine_handle<> mAwaitingCoroutine;
        
    protected:
        uint32_t mEvents;
        bool mSkipped{false};
    };
}

#endif