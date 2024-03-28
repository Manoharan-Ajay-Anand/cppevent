#ifndef CPPEVENT_FCGI_OUTPUT_CONTROL_HPP
#define CPPEVENT_FCGI_OUTPUT_CONTROL_HPP

#include "types.hpp"

#include <queue>
#include <coroutine>
#include <optional>

#include <cppevent_base/task.hpp>

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
    coroutine_opt& m_output_handle_opt;
    bool& m_error;

    bool await_ready();

    std::coroutine_handle<> await_suspend(std::coroutine_handle<> handle);

    void await_resume();
};

class output_control {
private:
    bool m_error = false;

    coroutine_opt m_output_handle_opt;

    std::queue<output_record> m_out_records;

public:

    awaitable_task<void> begin_res_task(socket& sock);

    fcgi_write_awaiter write(long m_type, long m_req_id, const void* m_src, long m_size);
};

}

#endif
