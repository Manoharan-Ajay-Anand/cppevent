#ifndef CPPEVENT_FCGI_OUTPUT_HPP
#define CPPEVENT_FCGI_OUTPUT_HPP

#include "output_control.hpp"

#include <string_view>

#include <cppevent_base/task.hpp>

namespace cppevent {

class socket;

class output {
private:
    const int m_req_id;
    const int m_type;
    output_control& m_control;
public:
    output(int req_id, int type, output_control& control);

    fcgi_write_awaiter write(const void* src, long size);
    fcgi_write_awaiter write(std::string_view s);
    fcgi_write_awaiter end();
};

}

#endif
