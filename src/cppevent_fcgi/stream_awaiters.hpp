#ifndef CPPEVENT_FCGI_STREAM_AWAITERS_HPP
#define CPPEVENT_FCGI_STREAM__AWAITERS_HPP

#include "types.hpp"

namespace cppevent {

class suspended_coro;

struct stream_update_awaiter {
    suspended_coro& m_producer;
    suspended_coro& m_consumer;
    long& m_remaining;

    bool await_ready();

    void await_suspend(std::coroutine_handle<> handle);

    void await_resume();
};

struct stream_readable_awaiter {
    suspended_coro& m_producer;
    suspended_coro& m_consumer;
    long& m_remaining;
    bool& m_ended;

    bool await_ready();

    std::coroutine_handle<> await_suspend(std::coroutine_handle<> handle);

    bool await_resume();
};

}

#endif
