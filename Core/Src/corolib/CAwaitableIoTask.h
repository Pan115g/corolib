/*
 * CAwaitableTask.h
 *
 *  Created on: May 8, 2025
 *      Author: pan.li
 */

#ifndef SRC_COROLIB_CAWAITABLEIOTASK_H_
#define SRC_COROLIB_CAWAITABLEIOTASK_H_

#include <coroutine>
#include <cstdint>
#include "CTaskScheduler.h"

namespace corolib {
    struct CAwaitableIoTaskBase
    {
        using Callback = void(CAwaitableIoTaskBase*, uint32_t);
        explicit CAwaitableIoTaskBase(Callback* callback) : mCallback(callback) {}
        Callback* mCallback;
    };
    /*
    * AwaitableTask is a base class for all awaitable tasks, such as I/O operations.
    T is the derived class that implements the actual task.
    */
    template <typename T>
    class CAwaitableIoTask : public CAwaitableIoTaskBase
    {
    public:
        explicit CAwaitableIoTask(CTaskScheduler& scheduler) noexcept : CAwaitableIoTaskBase{&CAwaitableIoTask<T>::onTaskCompleted},
        mAwaitingCoroutine{}, mEvents{0}, mScheduler{scheduler} {}

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
            CAwaitableIoTaskBase* task,
            uint32_t events) noexcept
        {
            auto awaitableTask = static_cast<CAwaitableIoTask<T>*>(task);
            awaitableTask->mEvents = events;
            awaitableTask->mScheduler.schedule(awaitableTask->mAwaitingCoroutine);
        }

        std::coroutine_handle<> mAwaitingCoroutine;

    protected:
        uint32_t mEvents;
        bool mSkipped{false};
        CTaskScheduler& mScheduler;
    };
} /* namespace corolib */

#endif /* SRC_COROLIB_CAWAITABLEIOTASK_H_ */
