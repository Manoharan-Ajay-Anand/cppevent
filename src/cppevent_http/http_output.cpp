#include "http_output.hpp"

#include <array>
#include <charconv>
#include <stdexcept>

cppevent::http_output::http_output(HTTP_VERSION version, socket& sock): m_version(version),
                                                                        m_sock(sock) {
}

void cppevent::http_output::set_status(HTTP_STATUS status) {
    m_status_line = get_version_string(m_version);
    m_status_line.push_back(' ');
    m_status_line.append(get_status_reason_phrase(status));
}

void cppevent::http_output::set_header(std::string_view name, std::string_view value) {
    auto it = m_header_lookup.find(name);
    if (it != m_header_lookup.end()) {
        it->second = value;
    } else if (!name.empty() && !value.empty()) {
        http_string key = name;
        std::string_view key_v = key.get_view();
        m_header_names.push_back(std::move(key));
        m_header_lookup[key_v] = value;
    }
}
