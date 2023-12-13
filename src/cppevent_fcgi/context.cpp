#include "context.hpp"

#include <algorithm>

constexpr std::string_view GET_STR = "GET";
constexpr std::string_view POST_STR = "POST";

constexpr std::string_view DOCUMENT_URI_STR = "DOCUMENT_URI";

constexpr std::string_view CONTENT_LENGTH_STR = "CONTENT_LENGTH";
constexpr std::string_view CONTENT_TYPE_STR = "CONTENT_TYPE";

constexpr std::string_view REQUEST_METHOD_STR = "REQUEST_METHOD";

constexpr std::string_view QUERY_STRING_STR = "QUERY_STRING";

const std::unordered_map<std::string_view, cppevent::REQUEST_METHOD> method_map = {
    { GET_STR, cppevent::REQUEST_METHOD::GET },
    { POST_STR, cppevent::REQUEST_METHOD::POST },
};

cppevent::context::context(const std::unordered_map<std::string_view,
                                                    std::string_view>& params): m_params(params) {
    m_path = m_params.at(DOCUMENT_URI_STR);
    m_req_method = method_map.at(m_params.at(REQUEST_METHOD_STR));
    m_content_length = 0;

    auto type_it = m_params.find(CONTENT_TYPE_STR);
    if (type_it != m_params.end()) {
        m_content_type = type_it->second;
    }

    auto len_it = m_params.find(CONTENT_LENGTH_STR);
    if (len_it != m_params.end()) {
        m_content_length = std::stol(std::string { len_it->second });
    }

    auto query_it = m_params.find(QUERY_STRING_STR);
    if (query_it != m_params.end()) {
        set_query_params(query_it->second);
    }
}

void cppevent::context::set_query_params(std::string_view query_str) {
    long last_eq = -1;
    long last_amp = -1;
    for (long i = 0; i <= query_str.size(); ++i) {
        if (i == query_str.size() || query_str[i] == '&') {
            long key_start = last_amp + 1;
            std::size_t key_size = static_cast<std::size_t>(std::max(0L, last_eq - key_start));
            long val_start = last_eq + 1;
            std::size_t val_size = static_cast<std::size_t>(std::max(0L, i - val_start));
            if (key_size > 0) {
                std::string_view key = { query_str.data() + key_start, key_size };
                std::string_view val = { query_str.data() + val_start, val_size };
                m_query_params.insert(std::pair { key, val });
            }
            last_amp = i;
        } else if (query_str[i] == '=') {
            last_eq = i;
        }
    }
}

const std::multimap<std::string_view,
                    std::string_view>& cppevent::context::get_query_params() const {
    return m_query_params;
}

std::optional<std::string_view> cppevent::context::get_param(std::string_view key) const {
    auto it = m_params.find(key);
    if (it == m_params.end()) {
        return {};
    }
    return { it->second };
}

std::optional<std::string_view> cppevent::context::get_path_segment(std::string_view name) const {
    auto it = m_path_segments.find(name);
    if (it == m_path_segments.end()) {
        return {};
    }
    return { it->second };
}

void cppevent::context::set_path_segment(std::string_view name, std::string_view val) {
    m_path_segments[name] = val;
}

std::string_view cppevent::context::get_path() const {
    return m_path;
}

std::string_view cppevent::context::get_content_type() const {
    return m_content_type;
}

long cppevent::context::get_content_len() const {
    return m_content_length;
}

cppevent::REQUEST_METHOD cppevent::context::get_req_method() const {
    return m_req_method;
}
