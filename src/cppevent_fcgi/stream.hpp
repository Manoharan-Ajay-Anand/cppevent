#ifndef CPPEVENT_FCGI_STREAM_HPP
#define CPPEVENT_FCGI_STREAM_HPP

#include "stream_awaiters.hpp"

#include <coroutine>
#include <optional>

#include <cppevent_base/task.hpp>

namespace cppevent {

class socket;

class event_loop;

class stream {
private:
    socket& m_conn;
    event_loop& m_loop;

    std::optional<std::coroutine_handle<>> m_producer;
    std::optional<std::coroutine_handle<>> m_consumer;
    
    long m_remaining;
    bool m_ended;

public:
    stream(socket& conn, event_loop& loop);

    stream_readable_awaiter can_read();
    awaitable_task<long> read(void* dest, long size, bool read_fully);
    awaitable_task<long> skip(long size, bool skip_fully);

    stream_update_awaiter update(long remaining);
};

}

#endif
