#ifndef SRC_COROLIB_CTASKSCHEDULER_H_
#define SRC_COROLIB_CTASKSCHEDULER_H_

#include <coroutine>
#include <atomic>
#include <memory>
#include "FreeRTOS.h"
#include "task.h"

namespace corolib
{
    class CTaskScheduler
    {
    public:
        explicit CTaskScheduler();
    
        ~CTaskScheduler();

        bool isInitialized(){return mInitialized;}

        void schedule(std::coroutine_handle<> operation) noexcept;

    private:
    
        static void runThread(void* self) noexcept;
                
        void tryEnqueue(std::coroutine_handle<> operation);

        std::coroutine_handle<> tryDequeue() noexcept;
    

        void shutdown() ;
    
    private:
        TaskHandle_t mThread;
        bool mInitialized{false};
    
        std::atomic<std::size_t> mQueueHead;
        std::atomic<std::size_t> mQueueTail;
        constexpr static std::size_t mQueueCapacity = 256; //must be a power of 2
        constexpr static std::size_t mQueueMask = mQueueCapacity - 1; //must be mGlobalQueueCapacity - 1
        std::unique_ptr<std::atomic<std::coroutine_handle<>>[]> mQueue;
        std::atomic<bool> mShutdownRequested;
    };
    
}

#endif
