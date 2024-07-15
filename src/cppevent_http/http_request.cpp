#include "http_request.hpp"

#include "util.hpp"

#include <unordered_map>
#include <algorithm>
#include <iostream>

void cppevent::http_request::process_uri(std::string_view uri) {
    m_uri = uri;

    auto q_it = m_uri.begin();
    for (; q_it != m_uri.end() && *q_it != '?'; ++q_it);
    
    m_path = { m_uri.begin(), q_it };
    m_path_segments = split_string(m_path, '/');

    if (q_it != m_uri.end()) {
        m_query = std::string_view { q_it + 1, m_uri.end() };
        m_query_params = retrieve_params(m_query);
    }
}

namespace cppevent {

std::unordered_map<std::string_view, HTTP_METHOD> method_map = {
    { "GET", HTTP_METHOD::GET },
    { "POST", HTTP_METHOD::POST },
    { "HEAD", HTTP_METHOD::HEAD },
    { "OPTIONS", HTTP_METHOD::OPTIONS }
};

std::unordered_map<std::string_view, HTTP_VERSION> version_map = {
    { "HTTP/1.0", HTTP_VERSION::HTTP_1_0 },
    { "HTTP/1.1", HTTP_VERSION::HTTP_1_1 }
};

}

bool cppevent::http_request::process_req_line(std::string_view line) {
    std::vector<std::string_view> req_segments = split_string(line, ' ');
    if (req_segments.size() != 3) return false;

    auto method_it = method_map.find(req_segments[0]);
    if (method_it == method_map.end()) return false;
    m_method = method_it->second;
    
    process_uri(req_segments[1]);
    
    auto version_it = version_map.find(req_segments[2]);
    if (version_it == version_map.end()) return false;
    m_version = version_it->second;

    return true;
}

bool cppevent::http_request::process_header_line(std::string_view line) {
    size_t pos = line.find(':');
    if (pos == std::string_view::npos) {
        return false;
    }

    http_string key = trim_string(line.substr(0, pos));
    std::string_view key_v = key.get_view();
    m_header_names.push_back(std::move(key));

    std::string value { trim_string(line.substr(pos + 1)) };
    if (key_v.empty() || value.empty()) return false;

    m_header_lookup[key_v] = std::move(value);
    return true;
}

cppevent::HTTP_METHOD cppevent::http_request::get_method() const {
    return m_method;
}

cppevent::HTTP_VERSION cppevent::http_request::get_version() const {
    return m_version;
}

std::string_view cppevent::http_request::get_path() const {
    return m_path;
}

const std::vector<std::string_view>& cppevent::http_request::get_path_segments() const {
    return m_path_segments;
}

std::string_view cppevent::http_request::get_query() const {
    return m_query;
}

std::optional<std::string_view> cppevent::http_request::get_query_param(std::string_view key) const {
    auto it = m_query_params.find(key);
    if (it == m_query_params.end()) {
        return {};
    }
    return { it->second };
}

std::vector<std::string_view> cppevent::http_request::get_multi_query_param(std::string_view key) const {
    std::vector<std::string_view> result;
    auto p = m_query_params.equal_range(key);
    for (auto it = p.first; it != p.second; ++it) {
        result.push_back(it->second);
    }
    return result;
}

std::optional<std::string_view> cppevent::http_request::get_header(std::string_view key) const {
    auto it = m_header_lookup.find(key);
    if (it == m_header_lookup.end()) {
        return {};
    }
    return { it->second };
}
