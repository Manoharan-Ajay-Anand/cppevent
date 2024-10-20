#ifndef CPPEVENT_BASE_TASK_HPP
#define CPPEVENT_BASE_TASK_HPP

#include <optional>
#include <coroutine>
#include <exception>

namespace cppevent {

struct final_awaiter {
    bool m_linked;
    std::coroutine_handle<> m_outer;

    bool await_ready() noexcept {
        return !m_linked;
    }

    std::coroutine_handle<> await_suspend(std::coroutine_handle<> handle) noexcept {
        return m_outer;
    }

    void await_resume() noexcept {}
};

template <typename T = void>
struct task {
    struct promise_type {
        std::optional<T> m_val_opt;
        std::exception_ptr m_exception;

        std::coroutine_handle<> m_outer = std::noop_coroutine();
        
        task<T> get_return_object() {
            return { std::coroutine_handle<promise_type>::from_promise(*this) };
        }

        std::suspend_never initial_suspend() { return {}; }
        
        final_awaiter final_suspend() noexcept { 
            return { true, m_outer };
        }
        
        void unhandled_exception() {
            m_exception = std::current_exception();
        }
        
        template <std::convertible_to<T> U>
        void return_value(U&& val) {
            m_val_opt = std::forward<U>(val);
        }
    };

private:
    std::coroutine_handle<promise_type> m_handle;

public:
    task(std::coroutine_handle<promise_type> handle): m_handle(handle) {}

    ~task() {
        m_handle.destroy();
    }

    task(const task&) = delete;
    task& operator=(const task&) = delete;

    task(task&&) = delete;
    task& operator=(task&&) = delete;

    bool await_ready() noexcept {
        return m_handle.done();
    }
    
    void await_suspend(std::coroutine_handle<> handle) {
        m_handle.promise().m_outer = handle;
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
struct task<> {
    struct promise_type {
        std::exception_ptr m_exception;

        bool m_linked = true;
        std::coroutine_handle<> m_outer = std::noop_coroutine();
        
        task<> get_return_object() {
            return { std::coroutine_handle<promise_type>::from_promise(*this) };
        }

        std::suspend_never initial_suspend() { return {}; }
        
        final_awaiter final_suspend() noexcept { 
            return { m_linked, m_outer };
        }
        
        void unhandled_exception() {
            m_exception = std::current_exception();
        }
        
        void return_void() {}
    };

private:
    std::coroutine_handle<promise_type> m_handle;
    bool m_linked = true;

public:
    task(std::coroutine_handle<promise_type> handle): m_handle(handle) {}

    ~task() {
        if (m_linked) {
            m_handle.destroy();
        }
    }

    task(const task&) = delete;
    task& operator=(const task&) = delete;

    task(task&&) = delete;
    task& operator=(task&&) = delete;

    bool await_ready() noexcept {
        return m_handle.done();
    }
    
    void await_suspend(std::coroutine_handle<> handle) {
        m_handle.promise().m_outer = handle;
    }
    
    void await_resume() {
        promise_type& promise = m_handle.promise();
        if (promise.m_exception) {
            std::rethrow_exception(promise.m_exception);
        }
    }

    void unlink() {
        m_handle.promise().m_linked = false;
        m_linked = false;
    }
};

}

#endif
