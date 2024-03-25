#ifndef CPPEVENT_FCGI_OUTPUT_HPP
#define CPPEVENT_FCGI_OUTPUT_HPP

#include <string_view>

#include <cppevent_base/task.hpp>

namespace cppevent {

class socket;

class output_control;

class output {
private:
    const int m_req_id;
    const int m_type;
    output_control& m_control;
    socket& m_sock;
public:
    output(int req_id, int type, output_control& control, socket& sock);

    awaitable_task<void> write(const void* src, long size);
    awaitable_task<void> write(std::string_view s);
    awaitable_task<void> end();
};

}

#endif
