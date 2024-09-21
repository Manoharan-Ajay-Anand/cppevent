#ifndef CPPEVENT_HTTP_HTTP_LINE_HPP
#define CPPEVENT_HTTP_HTTP_LINE_HPP

#include <cppevent_base/task.hpp>

#include <string>

namespace cppevent {

class socket;

struct http_line {
    std::string m_val;
    bool m_received;

    bool has_value() const;
    bool is_last_line() const;
};

task<http_line> read_http_line(socket& sock);

}

#endif
