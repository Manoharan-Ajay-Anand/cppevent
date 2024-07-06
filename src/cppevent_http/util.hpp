#ifndef CPPEVENT_HTTP_UTIL_HTTP
#define CPPEVENT_HTTP_UTIL_HTTP

#include <cppevent_base/task.hpp>

#include <string>

namespace cppevent {

class socket;

struct http_line {
    std::string m_val;
    bool m_received;
};

awaitable_task<http_line> read_http_line(socket& sock);

}

#endif
