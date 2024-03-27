#ifndef CPPEVENT_FCGI_REQUEST_HPP
#define CPPEVENT_FCGI_REQUEST_HPP

#include "stream.hpp"
#include "output.hpp"

#include <cppevent_base/async_signal.hpp>
#include <cppevent_base/task.hpp>

#include <coroutine>
#include <optional>

namespace cppevent {

class socket;

class event_loop;

class fcgi_handler;

class request {
private:
    stream m_params;
    stream m_stdin;

    output m_stdout;
    output m_endreq;

    awaitable_task<void> m_task;

    stream* get_stream(int type);

public:
    request(int id,
            bool* close_ctrl, bool close_conn,
            event_loop& loop, socket& conn,
            output_control& control, fcgi_handler& handler);

    stream_update_awaiter update(int type, long remaining);
};

}

#endif
