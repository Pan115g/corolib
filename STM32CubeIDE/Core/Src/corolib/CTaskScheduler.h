#ifndef SRC_COROLIB_CTASKSCHEDULER_H_
#define SRC_COROLIB_CTASKSCHEDULER_H_

#include <coroutine>
#include <atomic>
#include <memory>
#include "cmsis_os.h"

namespace corolib
{
    class CTaskScheduler
    {
    public:
        class CContextSwitchOperation
        {
        public:

            CContextSwitchOperation(CTaskScheduler& tp) noexcept : mScheduler(tp) {}

            bool await_ready() noexcept { return false; }
            void await_suspend(std::coroutine_handle<> awaitingCoroutine) noexcept
            {
                mScheduler.tryEnqueue(std::move(awaitingCoroutine));
                mScheduler.wakeUp();
            }
            void await_resume() noexcept {}

        private:
            CTaskScheduler& mScheduler;
        };

        explicit CTaskScheduler();
        CTaskScheduler(const CTaskScheduler&) = delete;

        ~CTaskScheduler();

        bool isInitialized(){return mInitialized;}

        CContextSwitchOperation schedule() noexcept
        {
            return CContextSwitchOperation(*this);
        }

        void tryEnqueue(std::coroutine_handle<> operation);

        void wakeUp();

    private:

        static void runThread(void* scheduler) noexcept;
                

        std::coroutine_handle<> tryDequeue() noexcept;
    

        void shutdown() ;
    
    private:
        bool mInitialized{false};
        osThreadId_t mThread;
        std::atomic<uint32_t> mQueueHead;
        std::atomic<uint32_t> mQueueTail;
        constexpr static uint32_t mQueueCapacity = 256; //must be a power of 2
        constexpr static uint32_t mQueueMask = mQueueCapacity - 1; //must be mGlobalQueueCapacity - 1
        std::unique_ptr<std::atomic<std::coroutine_handle<>>[]> mQueue;
        std::atomic<bool> mShutdownRequested;
    };
    
}

#endif
