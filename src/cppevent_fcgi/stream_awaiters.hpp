#ifndef CPPEVENT_FCGI_STREAM_AWAITERS_HPP
#define CPPEVENT_FCGI_STREAM__AWAITERS_HPP

#include "types.hpp"

namespace cppevent {

class event_loop;

struct stream_update_awaiter {
    coroutine_opt& m_producer;
    coroutine_opt& m_consumer;
    bool m_ended;

    bool await_ready();

    std::coroutine_handle<> await_suspend(std::coroutine_handle<> handle);

    void await_resume();
};

struct stream_readable_awaiter {
    coroutine_opt& m_producer;
    coroutine_opt& m_consumer;
    long& m_remaining;
    bool& m_ended;

    bool await_ready();

    std::coroutine_handle<> await_suspend(std::coroutine_handle<> handle);

    bool await_resume();
};

}

#endif
