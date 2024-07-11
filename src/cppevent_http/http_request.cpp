#include "http_request.hpp"

#include "util.hpp"

#include <unordered_map>

void cppevent::http_request::process_path(std::string_view path) {
    m_path = path;
    size_t question_pos = m_path.find('?');    
    m_path_segments = split_string(m_path.substr(0, question_pos), '/');
    if (question_pos < m_path.size()) {
        m_query_params = retrieve_params(m_path.substr(question_pos + 1));
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
    
    process_path(req_segments[1]);
    
    auto version_it = version_map.find(req_segments[2]);
    if (version_it == version_map.end()) return false;
    m_version = version_it->second;

    return true;
}

bool cppevent::http_request::process_header_line(std::string_view line) {
    return true;
}
