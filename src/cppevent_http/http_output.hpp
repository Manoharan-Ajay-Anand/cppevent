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

    header_map m_headers;

    bool m_headers_written = false;
    bool m_chunked_encoding = false;

    task<> raw_write(const void* src, long size);
    task<> raw_write(std::string_view sv);

    task<> write_headers();
public:
    http_output(socket& sock);

    void set_status(HTTP_STATUS status);
    void set_header(std::string_view name, std::string_view value);
    void set_content_length(long len);

    task<> write(const void* src, long size);
    task<> write(std::string_view sv);

    task<> end();
};

}

#endif
