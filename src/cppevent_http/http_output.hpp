#ifndef CPPEVENT_HTTP_HTTP_OUTPUT_HPP
#define CPPEVENT_HTTP_HTTP_OUTPUT_HPP

#include "types.hpp"

#include <cppevent_base/task.hpp>

#include <string>
#include <vector>
#include <unordered_map>

namespace cppevent {

class socket;

class http_output {
private:
    socket& m_sock;
    HTTP_STATUS m_status = HTTP_STATUS::OK;

    task<> raw_write(const void* src, long size);
    task<> raw_write(std::string_view sv);

    std::string m_headers_buf;
    bool m_headers_written = false;

public:
    http_output(socket& sock);

    http_output& status(HTTP_STATUS status);
    http_output& header(std::string_view name, std::string_view value);
    http_output& content_length(long len);

    task<> write(const void* src, long size);
    task<> write(std::string_view sv);
};

}

#endif
