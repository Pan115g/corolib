#ifndef ENTRYPOINTAWAITABLE_H
#define ENTRYPOINTAWAITABLE_H

#include <coroutine>
#include <utility>

namespace corolib
{
    /**
     * EntryPointAwaitable is a awaitable which destroy coroutine in destructor.
     * The final_suspend() method will never suspend, so the coroutine will be destroyed immediately after the coroutine is resumed.
     * This is useful for coroutines that are used as entry points for FireAndForget.
     */
    class EntryPointAwaitable
    {
    public:
        class promise_type 
        {
            public:
            promise_type() noexcept  {}
            
            EntryPointAwaitable get_return_object() noexcept 
            {
                return EntryPointAwaitable{std::coroutine_handle<promise_type>::from_promise(*this)};
            }

            std::suspend_always initial_suspend() noexcept 
            {
                return {};
            }

            void return_void() noexcept {}

            void unhandled_exception() noexcept 
            {
                while(1);
            }            

            std::suspend_never final_suspend() noexcept 
            {
                return {};
            }
        };

        EntryPointAwaitable(EntryPointAwaitable&& t) noexcept
        : mCoroHandle(std::exchange(t.mCoroHandle, {}))
        {}

        bool await_ready() noexcept 
        {
            return false;
        }

        bool await_suspend(std::coroutine_handle<> continuation) noexcept 
        {
            return false;
        }

        void await_resume()
        {
        }

        void resume() &&
        {
            mCoroHandle.resume();
        }
    private:
        explicit EntryPointAwaitable(std::coroutine_handle<promise_type> h) noexcept
        : mCoroHandle(h)
        {}

        std::coroutine_handle<promise_type> mCoroHandle;
    };
}

#endif
