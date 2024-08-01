#include "http_output.hpp"

#include "util.hpp"
#include "types.hpp"

#include <cppevent_net/socket.hpp>

#include <array>
#include <charconv>
#include <stdexcept>

cppevent::http_output::http_output(socket& sock): m_sock(sock) {
}

void cppevent::http_output::set_status(HTTP_STATUS status) {
    m_status = status;
}

cppevent::task<> cppevent::http_output::raw_write(const void* src, long size) {
    return m_sock.write(src, size);
}

cppevent::task<> cppevent::http_output::raw_write(std::string_view s) {
    return raw_write(s.data(), s.size());
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

constexpr long SERIALIZE_BUFFER_SIZE = 256;

void cppevent::http_output::set_content_length(long len) {
    if (len < 0) {
        throw std::runtime_error("http_output set_content_len: value less than 0");
    }

    std::array<char, SERIALIZE_BUFFER_SIZE> temp;
    std::to_chars_result result = std::to_chars(temp.begin(), temp.end(), len);
    if (result.ec == std::errc::value_too_large) {
        throw std::runtime_error("http_output set_content_len: value too large");
    }

    set_header("content-length", std::string_view { temp.begin(), result.ptr });
}

constexpr std::string_view HTTP_1_1_STR = "HTTP/1.1 ";

constexpr std::string_view COLON_SEPARATOR = ": ";
constexpr std::string_view CRLF_SEPARATOR = "\r\n";

constexpr std::string_view TRANSFER_ENCODING = "transfer-encoding";

cppevent::task<> cppevent::http_output::write_headers() {
    if (m_headers_written) {
        co_return;
    }

    auto t_it = m_headers.find(TRANSFER_ENCODING);
    m_chunked_encoding = t_it != m_headers.end() &&
                         find_case_insensitive(t_it->second, "chunked") != std::string_view::npos;
    
    co_await raw_write(HTTP_1_1_STR);
    co_await raw_write(get_status_reason_phrase(m_status));
    co_await raw_write(CRLF_SEPARATOR);

    for (auto& p : m_headers) {
        co_await raw_write(p.first);
        co_await raw_write(COLON_SEPARATOR);
        co_await raw_write(p.second);
        co_await raw_write(CRLF_SEPARATOR);
    }
    co_await raw_write(CRLF_SEPARATOR);

    m_headers_written = true;
}

cppevent::task<> cppevent::http_output::write(const void* src, long size) {
    if (size <= 0) {
        throw std::runtime_error("http_output write: size less than or equal 0");
    }
    
    co_await write_headers();

    if (m_chunked_encoding) {
        std::array<char, SERIALIZE_BUFFER_SIZE> temp;
        std::to_chars_result result = std::to_chars(temp.begin(), temp.end(), size, HEX_BASE);
        if (result.ec == std::errc::value_too_large) {
            throw std::runtime_error("http_output write: size too large");
        }
        co_await raw_write(temp.begin(), result.ptr - temp.begin());
        co_await raw_write(CRLF_SEPARATOR);
        co_await raw_write(src, size);
        co_await raw_write(CRLF_SEPARATOR);
    } else {
        co_await raw_write(src, size);
    }
}

cppevent::task<> cppevent::http_output::write(std::string_view sv) {
    return write(sv.data(), sv.size());
}

constexpr std::string_view LAST_CHUNK = "0\r\n\r\n";

cppevent::task<> cppevent::http_output::end() {
    co_await write_headers();
    
    if (m_chunked_encoding) {
        co_await raw_write(LAST_CHUNK);
    }
    co_await m_sock.flush();
}
