#ifndef CPPEVENT_FCGI_STREAM_AWAITERS_HPP
#define CPPEVENT_FCGI_STREAM__AWAITERS_HPP

#include <coroutine>
#include <optional>

namespace cppevent {

class event_loop;

struct stream_update_awaiter {
    std::optional<std::coroutine_handle<>>& m_producer;
    std::optional<std::coroutine_handle<>>& m_consumer;
    bool m_ended;

    bool await_ready();

    std::coroutine_handle<> await_suspend(std::coroutine_handle<> handle);

    void await_resume();
};

struct stream_readable_awaiter {
    std::optional<std::coroutine_handle<>>& m_producer;
    std::optional<std::coroutine_handle<>>& m_consumer;
    event_loop& m_loop;
    long& m_remaining;
    bool& m_ended;

    bool await_ready();

    std::coroutine_handle<> await_suspend(std::coroutine_handle<> handle);

    bool await_resume();
};

}

#endif
