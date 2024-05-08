#ifndef CPPEVENT_FCGI_OUTPUT_CONTROL_HPP
#define CPPEVENT_FCGI_OUTPUT_CONTROL_HPP

#include "types.hpp"

#include <queue>
#include <coroutine>

#include <cppevent_base/task.hpp>
#include <cppevent_base/suspended_coro.hpp>

namespace cppevent {

class socket;

struct output_record {
    long m_type;
    long m_req_id;
    const void* m_src;
    long m_size;
    std::coroutine_handle<> m_handle;
};

struct fcgi_write_awaiter {
    long m_type;
    long m_req_id;
    const void* m_src;
    long m_size;

    std::queue<output_record>& m_out_records;
    suspended_coro& m_suspended_output;
    bool& m_error;

    bool await_ready();

    std::coroutine_handle<> await_suspend(std::coroutine_handle<> handle);

    void await_resume();
};

class output_control {
private:
    bool m_error = false;

    suspended_coro m_suspended_output;

    std::queue<output_record> m_out_records;

public:

    awaitable_task<void> begin_res_task(socket& sock);

    fcgi_write_awaiter write(long m_type, long m_req_id, const void* m_src, long m_size);
};

}

#endif
