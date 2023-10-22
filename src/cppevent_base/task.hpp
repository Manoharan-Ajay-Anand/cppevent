#ifndef CPPEVENT_BASE_TASK_HPP
#define CPPEVENT_BASE_TASK_HPP

#include <coroutine>
#include <optional>
#include <exception>

namespace cppevent {

struct task {
    struct promise_type {
        task get_return_object() { return {}; }

        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        
        void unhandled_exception() {
            throw;
        }
        
        void return_void() {}
    };
};

template <typename T>
struct final_awaiter {
    bool await_ready() noexcept { return false; }

    std::coroutine_handle<> await_suspend(std::coroutine_handle<T> handle) noexcept {
        auto& waiting = handle.promise().m_waiting;
        if (waiting) {
            return waiting;
        }
        return std::noop_coroutine();
    }

    void await_resume() noexcept {}
};

template <typename T>
struct awaitable_task {
    struct promise_type {

        std::optional<T> m_val_opt;
        std::coroutine_handle<> m_waiting;
        std::exception_ptr m_exception;

        awaitable_task<T> get_return_object() {
            return { std::coroutine_handle<promise_type>::from_promise(*this) };
        }

        std::suspend_never initial_suspend() { return {}; }
        final_awaiter<promise_type> final_suspend() noexcept { return {}; }
        
        void unhandled_exception() {
            m_exception = std::current_exception();
        }
        
        void return_value(const T& val) {
            m_val_opt = val;
        }

        void return_value(T&& val) {
            m_val_opt = std::move(val);
        }
    };

private:
    std::coroutine_handle<promise_type> m_handle;

public:
    awaitable_task(std::coroutine_handle<promise_type> handle): m_handle(handle) {}

    ~awaitable_task() {
        m_handle.destroy();
    }

    awaitable_task(const awaitable_task&) = delete;
    awaitable_task& operator=(const awaitable_task&) = delete;

    awaitable_task(awaitable_task&&) = delete;
    awaitable_task& operator=(awaitable_task&&) = delete;

    bool await_ready() noexcept {
        return m_handle.done();
    }
    
    void await_suspend(std::coroutine_handle<> handle) {
        m_handle.promise().m_waiting = handle;
    }
    
    T&& await_resume() {
        promise_type& promise = m_handle.promise();
        if (promise.m_exception) {
            std::rethrow_exception(promise.m_exception);
        }
        return std::move(promise.m_val_opt).value();
    }
};

template<>
struct awaitable_task<void> {
    struct promise_type {

        std::coroutine_handle<> m_waiting;
        std::exception_ptr m_exception;

        awaitable_task<void> get_return_object() {
            return { std::coroutine_handle<promise_type>::from_promise(*this) };
        }

        std::suspend_never initial_suspend() { return {}; }
        final_awaiter<promise_type> final_suspend() noexcept { return {}; }
        
        void unhandled_exception() {
            m_exception = std::current_exception();
        }
        
        void return_void() {}
    };
private:
    std::coroutine_handle<promise_type> m_handle;

public:
    awaitable_task(std::coroutine_handle<promise_type> handle): m_handle(handle) {}

    ~awaitable_task() {
        m_handle.destroy();
    }

    awaitable_task(const awaitable_task&) = delete;
    awaitable_task& operator=(const awaitable_task&) = delete;

    awaitable_task(awaitable_task&&) = delete;
    awaitable_task& operator=(awaitable_task&&) = delete;

    bool await_ready() noexcept {
        return m_handle.done();
    }
    
    void await_suspend(std::coroutine_handle<> handle) {
        m_handle.promise().m_waiting = handle;
    }
    
    void await_resume() {
        promise_type& promise = m_handle.promise();
        if (promise.m_exception) {
            std::rethrow_exception(promise.m_exception);
        }
    }
};

}

#endif
