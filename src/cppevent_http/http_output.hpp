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
    HTTP_VERSION m_version;
    socket& m_sock;
    HTTP_STATUS m_status = HTTP_STATUS::OK;

    header_map m_headers;

    bool m_headers_flushed = false;

    awaitable_task<void> raw_write(const void* src, long size);
    awaitable_task<void> raw_write(std::string_view sv);

    awaitable_task<void> flush_headers();
public:
    http_output(HTTP_VERSION version, socket& sock);

    void set_status(HTTP_STATUS status);
    void set_header(std::string_view name, std::string_view value);
    void set_content_length(long len);
};

}

#endif
