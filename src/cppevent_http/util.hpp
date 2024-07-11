#ifndef CPPEVENT_HTTP_UTIL_HTTP
#define CPPEVENT_HTTP_UTIL_HTTP

#include <cppevent_base/task.hpp>

#include <string>
#include <vector>
#include <string_view>

namespace cppevent {

class socket;

struct http_line {
    std::string m_val;
    bool m_received;

    bool has_value() const;
    bool is_last_line() const;
};

awaitable_task<http_line> read_http_line(socket& sock);

std::vector<std::string_view> split_string(std::string_view s, char separator);

}

#endif
