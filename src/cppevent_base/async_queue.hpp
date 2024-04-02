#ifndef CPPEVENT_BASE_ASYNC_QUEUE_HPP
#define CPPEVENT_BASE_ASYNC_QUEUE_HPP

#include "event_loop.hpp"
#include "event_callback.hpp"
#include "task.hpp"

#include <queue>

namespace cppevent {

template <typename T>
struct async_queue_awaiter {
    std::queue<T>& m_items;
    event_callback& m_callback;

    bool await_ready() {
        return !m_items.empty();
    }
    
    void await_suspend(std::coroutine_handle<> handle) {
        m_callback.set_handler([handle](e_status stat) {
            handle.resume();
        });
    }
    
    long await_resume() {
        return m_items.size();
    }
};

template <typename T>
class async_queue {
private:
    std::queue<T> m_items;
    event_loop& m_loop;
    event_callback m_callback;
public:
    async_queue(event_loop& loop): m_loop(loop), m_callback(loop.get_event_callback()) {
    }

    async_queue_awaiter<T> await_items() {
        return { m_items, m_callback };
    }

    T& front() {
        return m_items.front();
    }

    void pop() {
        m_items.pop();
    }

    template <std::convertible_to<T> U>
    void push(U&& item) {
        if (m_items.empty()) {
            m_loop.add_event({ m_callback.get_id(), 0 });
        }
        m_items.push(std::forward<U>(item));
    }
};

}

#endif
