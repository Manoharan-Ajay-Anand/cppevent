#ifndef CPPEVENT_FCGI_STREAM_HPP
#define CPPEVENT_FCGI_STREAM_HPP

#include "stream_awaiters.hpp"

#include <string>
#include <coroutine>
#include <optional>

#include <cppevent_base/task.hpp>
#include <cppevent_base/suspended_coro.hpp>

namespace cppevent {

class socket;

class stream {
private:
    socket& m_conn;

    suspended_coro m_producer;
    suspended_coro m_consumer;
    
    long m_remaining = 0;
    bool m_ended = false;

public:
    stream(socket& conn);

    stream_readable_awaiter can_read();
    awaitable_task<long> read(void* dest, long size, bool read_fully);
    awaitable_task<long> read(std::string& dest, long size, bool read_fully);
    awaitable_task<long> skip(long size, bool skip_fully);

    stream_update_awaiter update(long remaining);
};

}

#endif
