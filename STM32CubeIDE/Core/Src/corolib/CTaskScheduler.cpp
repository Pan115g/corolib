#include <CTaskScheduler.h>
#include "cmsis_os.h"

namespace corolib
{
    CTaskScheduler::CTaskScheduler() :
		mQueueHead{0},
		mQueueTail{0},
		mQueue{std::make_unique<std::atomic<std::coroutine_handle<>>[]>(mQueueCapacity)},
		mShutdownRequested{false}
	{
	    //mInitialized = xTaskCreate(CTaskScheduler::runThread,"Task scheduler", 500, this,
	      //      24, &mThread) == pdPASS;
	    const osThreadAttr_t defaultTask_attributes = {
	      .name = "schedulerTask",
	      .stack_size = 128 * 4,
	      .priority = (osPriority_t) osPriorityNormal,
	    };
	    osThreadNew(CTaskScheduler::runThread, this, &defaultTask_attributes);

	}

	CTaskScheduler::~CTaskScheduler()
	{	
		shutdown();
	}

	void CTaskScheduler::shutdown()
	{
		mShutdownRequested.store(1, std::memory_order_relaxed);


	}


	void CTaskScheduler::runThread(void* scheduler) noexcept
	{
	    CTaskScheduler* self = static_cast<CTaskScheduler*>(scheduler);
		while(true)
		{
			// try to dequeue a task from the global queue
			auto operation = self->tryDequeue();
			if (operation)
			{
				operation.resume();
			}

			if(self->mShutdownRequested.load(std::memory_order_relaxed))
			{
				break;
			}
		}
	}

	void CTaskScheduler::tryEnqueue(std::coroutine_handle<> operation)
	{
		while (true)
		{
			uint32_t tail = mQueueTail.load(std::memory_order_acquire);
			uint32_t head = mQueueHead.load(std::memory_order_acquire);
			if ((tail - head) < mQueueCapacity)
			{
				if (mQueueTail.compare_exchange_strong(tail, tail + 1, std::memory_order_seq_cst, std::memory_order_relaxed))
				{
					mQueue[tail & mQueueMask].store(std::move(operation), std::memory_order_seq_cst);

					return;
				}
			}
			else
			{
			    //TODO: error handling
			    configASSERT(0);
			    return;
			}
		}
	}

	std::coroutine_handle<> CTaskScheduler::tryDequeue() noexcept
	{
		while (true)
		{
		    uint32_t head = mQueueHead.load(std::memory_order_acquire);
		    uint32_t tail = mQueueTail.load(std::memory_order_acquire);
			if (head < tail)
			{
				std::coroutine_handle<> operation = mQueue[head & mQueueMask].load(std::memory_order_seq_cst);
				if (mQueueHead.compare_exchange_strong(head, head + 1, std::memory_order_seq_cst, std::memory_order_relaxed))
				{

					return operation;
				}
			}
			else
			{
				return std::coroutine_handle<> {};
			}
		}
	}
}
