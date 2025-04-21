#ifndef AWAITABLE_H_
#define AWAITABLE_H_

#include <coroutine>
#include <utility>
#include <exception>
#include <atomic>

namespace corolib
{
    class promise_type_base
    {
        public:
        promise_type_base() noexcept : mContinuation{}, mState{false} {}  

        //noexcept avoid double-free of the coroutine frame see https://lewissbaker.github.io/2018/09/05/understanding-the-promise-type
        std::suspend_always initial_suspend() noexcept
        {
            return {};
        }

        void unhandled_exception() noexcept
        {
            //TODO: Handle exception considering firmware
            std::terminate();
        }

        struct final_awaiter
        {
            bool await_ready() noexcept
            {
                return false;
            }

            template <typename PromiseType>
            void await_suspend(std::coroutine_handle<PromiseType> h) noexcept
            {
                promise_type_base& promise = h.promise();

                if (promise.mState.exchange(true, std::memory_order_acq_rel))
                {
                    promise.mContinuation.resume();
                }
            }

            void await_resume() noexcept {}
        };

        final_awaiter final_suspend() noexcept
        {
            return {};
        }

        bool try_set_continuation(std::coroutine_handle<> continuation)
        {
            mContinuation = continuation;
            return !mState.exchange(true, std::memory_order_acq_rel);
        }

        private:
        std::coroutine_handle<> mContinuation;
        std::atomic<bool> mState;
    };

    template <typename T = void>
    class Awaitable;

    template <typename T>
    class awaitable_promise_type : public promise_type_base
    {
        public:
        awaitable_promise_type() noexcept : promise_type_base{} {}

        Awaitable<T> get_return_object() noexcept
        {
            return Awaitable<T>{std::coroutine_handle<awaitable_promise_type>::from_promise(*this)};
        }

        void return_value(T value) noexcept
        {
            mValue = value;
        }

        T result() noexcept
        {
            return mValue;
        }

        private:
        T mValue;
    };

    template <>
    class awaitable_promise_type<void> : public promise_type_base
    {
        public:
        awaitable_promise_type() noexcept : promise_type_base{} {}

        Awaitable<void> get_return_object() noexcept;

        void return_void() noexcept
        {
        }
    };

    template <typename T>
    class Awaitable
    {
    public:

        using promise_type = awaitable_promise_type<T>;

        explicit Awaitable(std::coroutine_handle<promise_type> h) noexcept
        : mCoroHandle(h)
        {}

        Awaitable(Awaitable&& t) noexcept
        : mCoroHandle(std::exchange(t.mCoroHandle, {}))
        {}

        ~Awaitable() {
        if (mCoroHandle)
            mCoroHandle.destroy();
        }

        class Awaiter
        {
            public:
            bool await_ready() noexcept
            {
                return false;
            }

            bool await_suspend(std::coroutine_handle<> continuation) noexcept
            {
                coro_.resume();
                return coro_.promise().try_set_continuation(continuation);
            }

            decltype(auto) await_resume()
            {
                if constexpr (std::is_same_v<T, void>)
                {
                    return;
                }
                else
                {
                    return coro_.promise().result();
                }
            }

            private:
            friend Awaitable<T>;
            explicit Awaiter(std::coroutine_handle<promise_type> h) noexcept
            : coro_(h)
            {}

            std::coroutine_handle<promise_type> coro_;
        };

        Awaiter operator co_await() && noexcept
        {
            return Awaiter{mCoroHandle};
        }

        void resume()
        {
            if (not mCoroHandle.done())
            {
                mCoroHandle.resume();
            }
        }

    private:
        std::coroutine_handle<promise_type> mCoroHandle;
    };

    Awaitable<void> awaitable_promise_type<void>::get_return_object() noexcept
    {
        return Awaitable<void>{std::coroutine_handle<awaitable_promise_type>::from_promise(*this)};
    }
}
#endif