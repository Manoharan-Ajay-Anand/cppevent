#ifndef CPPEVENT_HTTP_HTTP_REQUEST_HPP
#define CPPEVENT_HTTP_HTTP_REQUEST_HPP

#include <string_view>
#include <vector>
#include <map>

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

    std::multimap<std::string_view, std::string_view> m_query_params;

    void process_path(std::string_view path);
public:
    bool process_req_line(std::string_view line);
    bool process_header_line(std::string_view line);
};

}

#endif
