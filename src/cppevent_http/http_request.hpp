#ifndef CPPEVENT_HTTP_HTTP_REQUEST_HPP
#define CPPEVENT_HTTP_HTTP_REQUEST_HPP

#include "types.hpp"

#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <unordered_map>
#include <optional>

namespace cppevent {

class http_request {
private:
    HTTP_METHOD m_method;
    HTTP_VERSION m_version;

    std::string m_uri;

    std::string_view m_path;
    std::unordered_map<std::string_view, std::string_view> m_path_params;

    std::string_view m_query;
    std::multimap<std::string_view, std::string_view> m_query_params;

    header_map m_headers;

    void process_uri(std::string_view uri);

public:
    bool process_req_line(std::string_view line);
    bool process_header_line(std::string_view line);

    HTTP_METHOD get_method() const;
    HTTP_VERSION get_version() const;

    std::string_view get_path() const;
    void set_path_params(const std::unordered_map<std::string_view, std::string_view>& params);
    std::optional<std::string_view> get_path_param(std::string_view key) const;

    std::string_view get_query() const;
    std::optional<std::string_view> get_query_param(std::string_view key) const;
    std::vector<std::string_view> get_multi_query_param(std::string_view key) const;

    std::optional<std::string_view> get_header(std::string_view key) const;

    bool is_close_conn() const;
};

}

#endif
