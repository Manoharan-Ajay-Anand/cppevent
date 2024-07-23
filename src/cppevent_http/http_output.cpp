#include "http_output.hpp"

#include <cppevent_net/socket.hpp>

#include <array>
#include <charconv>
#include <stdexcept>

cppevent::http_output::http_output(HTTP_VERSION version, socket& sock): m_version(version),
                                                                        m_sock(sock) {
}

void cppevent::http_output::set_status(HTTP_STATUS status) {
    m_status = status;
}

cppevent::awaitable_task<void> cppevent::http_output::raw_write(const void* src, long size) {
    return m_sock.write(src, size);
}

cppevent::awaitable_task<void> cppevent::http_output::raw_write(std::string_view s) {
    return raw_write(s.data(), s.size());
}

constexpr std::string_view SPACE_SEPARATOR = " ";
constexpr std::string_view COLON_SEPARATOR = ": ";
constexpr std::string_view CRLF_SEPARATOR = "\r\n";

cppevent::awaitable_task<void> cppevent::http_output::flush_headers() {
    co_await raw_write(get_version_string(m_version));
    co_await raw_write(SPACE_SEPARATOR);
    co_await raw_write(get_status_reason_phrase(m_status));
    co_await raw_write(CRLF_SEPARATOR);

    for (auto& p : m_headers) {
        co_await raw_write(p.first);
        co_await raw_write(COLON_SEPARATOR);
        co_await raw_write(p.second);
        co_await raw_write(CRLF_SEPARATOR);
    }
    co_await raw_write(CRLF_SEPARATOR);

    m_headers_flushed = true;
}

void cppevent::http_output::set_header(std::string_view name, std::string_view value) {
    if (name.empty() || value.empty()) return;

    auto it = m_headers.find(name);
    if (it != m_headers.end()) {
        it->second = value;
    } else {
        m_headers[std::string { name }] = value;
    }
}

void cppevent::http_output::set_content_len(long len) {
    if (len < 0) {
        throw std::runtime_error("http_output set_content_len: len < 0");
    }

    std::array<char, 50> arr;
    std::to_chars_result result = std::to_chars(arr.begin(), arr.end(), len);
    if (result.ec != std::errc {}) {
        throw std::runtime_error("http_output set_content_len: failed to serialize len");
    }

    set_header("content-length", std::string_view { arr.begin(), result.ptr });
}
