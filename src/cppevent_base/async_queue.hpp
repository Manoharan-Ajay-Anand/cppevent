#ifndef CPPEVENT_BASE_ASYNC_QUEUE_HPP
#define CPPEVENT_BASE_ASYNC_QUEUE_HPP

#include "event_loop.hpp"
#include "event_callback.hpp"
#include "task.hpp"

#include <queue>

namespace cppevent {

template <typename T>
struct async_queue_awaiter {
private:
    std::queue<T>& m_items;
    std::coroutine_handle<>& m_handle;
public:
    async_queue_awaiter(std::queue<T>& items, std::coroutine_handle<>& handle): m_items(items),
                                                                                m_handle(handle) {
    }

    bool await_ready() { return !m_items.empty(); }
    
    void await_suspend(std::coroutine_handle<> handle) {
        m_handle = handle;
    }
    
    long await_resume() {
        m_handle = std::noop_coroutine(); 
        return m_items.size();
    }
};

template <typename T>
class async_queue {
private:
    std::queue<T> m_items;
    event_loop& m_loop;
    std::coroutine_handle<> m_handle;
public:
    async_queue(event_loop& loop): m_loop(loop) {
    }

    async_queue_awaiter<T> await_items() {
        return { m_items, m_handle };
    }

    T& front() {
        return m_items.front();
    }

    void pop() {
        m_items.pop();
    }

    void push(const T& item) {
        if (m_items.empty()) {
            m_loop.add_op([handle = m_handle]() { handle.resume(); });
        }
        m_items.push(item);
    }

    void push(T&& item) {
        if (m_items.empty()) {
            m_loop.add_op([handle = m_handle]() { handle.resume(); });
        }
        m_items.push(std::move(item));
    }
};

}

#endif
