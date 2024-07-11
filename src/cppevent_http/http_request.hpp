#ifndef CPPEVENT_HTTP_HTTP_REQUEST_HPP
#define CPPEVENT_HTTP_HTTP_REQUEST_HPP

#include <string_view>
#include <vector>

namespace cppevent {

struct http_line;

enum class HTTP_METHOD {
    GET,
    POST,
    HEAD,
    OPTIONS
};

enum class HTTP_VERSION {
    HTTP_1_0,
    HTTP_1_1
};

class http_request {
private:
    HTTP_METHOD m_method;
    HTTP_VERSION m_version;

    std::string_view m_path;
    std::vector<std::string_view> m_path_segments;

    bool process_path();
public:
    bool process_req_line(const http_line& line);
    bool process_header_line(const http_line& line);
};

}

#endif
