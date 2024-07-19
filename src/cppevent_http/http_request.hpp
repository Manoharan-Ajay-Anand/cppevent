#ifndef CPPEVENT_HTTP_HTTP_REQUEST_HPP
#define CPPEVENT_HTTP_HTTP_REQUEST_HPP

#include "http_string.hpp"
#include "types.hpp"

#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <unordered_map>
#include <optional>

namespace cppevent {

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

    std::string m_uri;
    std::string_view m_path;
    std::string_view m_query;
    std::vector<std::string_view> m_path_segments;
    std::multimap<std::string_view, std::string_view> m_query_params;

    std::vector<http_string> m_header_names;
    header_map m_header_lookup;

    void process_uri(std::string_view uri);
public:
    bool process_req_line(std::string_view line);
    bool process_header_line(std::string_view line);

    HTTP_METHOD get_method() const;
    HTTP_VERSION get_version() const;

    std::string_view get_path() const;
    const std::vector<std::string_view>& get_path_segments() const;

    std::string_view get_query() const;
    std::optional<std::string_view> get_query_param(std::string_view key) const;
    std::vector<std::string_view> get_multi_query_param(std::string_view key) const;

    std::optional<std::string_view> get_header(std::string_view key) const;
};

}

#endif