#ifndef CPPEVENT_BASE_ASYNC_QUEUE_HPP
#define CPPEVENT_BASE_ASYNC_QUEUE_HPP

#include "event_loop.hpp"
#include "event_callback.hpp"
#include "task.hpp"

#include <queue>

namespace cppevent {

template <typename T>
class async_queue {
private:
    std::queue<T> m_items;
    event_loop& m_loop;
    event_callback m_callback;
public:
    async_queue(event_loop& loop): m_loop(loop), m_callback(loop.get_event_callback()) {
    }

    awaitable_task<long> await_items() {
        while (m_items.empty()) {
            co_await m_callback.await_status();
        }
        co_return m_items.size();
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
