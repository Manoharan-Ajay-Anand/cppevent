#ifndef CPPEVENT_HTTP_UTIL_HTTP
#define CPPEVENT_HTTP_UTIL_HTTP

#include <cppevent_base/task.hpp>

#include <string>
#include <vector>
#include <string_view>
#include <map>

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

std::multimap<std::string_view, std::string_view> retrieve_params(std::string_view s);

std::string_view trim_string(std::string_view s);

}

#endif
