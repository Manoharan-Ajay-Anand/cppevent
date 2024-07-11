#include "http_request.hpp"

#include "util.hpp"

#include <unordered_map>

bool cppevent::http_request::process_path() {
    if (!m_path.starts_with('/')) return false;

    long question_pos;
    for (question_pos = 0;
         question_pos < m_path.size() && m_path[question_pos] != '?';
         ++question_pos);
    
    m_path_segments = split_string(m_path.substr(0, question_pos), '/');
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

bool cppevent::http_request::process_req_line(const http_line& line) {
    std::string_view val = line.m_val;
    std::vector<std::string_view> req_segments = split_string(val, ' ');
    if (req_segments.size() != 3) return false;

    auto method_it = method_map.find(req_segments[0]);
    if (method_it == method_map.end()) return false;
    m_method = method_it->second;
    
    m_path = req_segments[1];
    if (!process_path()) return false;

    auto version_it = version_map.find(req_segments[2]);
    if (version_it == version_map.end()) return false;
    m_version = version_it->second;

    return true;
}

bool cppevent::http_request::process_header_line(const http_line& line) {
    return true;
}
