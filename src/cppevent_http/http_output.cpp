#include "http_output.hpp"

#include "util.hpp"
#include "types.hpp"

#include <cppevent_net/socket.hpp>

#include <array>
#include <charconv>
#include <stdexcept>

cppevent::http_output::http_output(socket& sock): m_sock(sock) {
}

cppevent::task<> cppevent::http_output::raw_write(const void* src, long size) {
    return m_sock.write(src, size);
}

cppevent::task<> cppevent::http_output::raw_write(std::string_view s) {
    return raw_write(s.data(), s.size());
}

cppevent::http_output& cppevent::http_output::status(HTTP_STATUS status) {
    m_status = status;
    return *this;
}

constexpr std::string_view COLON_SEPARATOR = ": ";
constexpr std::string_view CRLF_SEPARATOR = "\r\n";

cppevent::http_output& cppevent::http_output::header(std::string_view name, std::string_view value) {
    if (!name.empty() && !value.empty()) {
        m_headers_buf.append(name)
                     .append(COLON_SEPARATOR)
                     .append(value)
                     .append(CRLF_SEPARATOR);
    }

    return *this;
}

constexpr long SERIALIZE_BUFFER_SIZE = 256;

cppevent::http_output& cppevent::http_output::content_length(long len) {
    if (len < 0) {
        throw std::runtime_error("http_output set_content_len: value less than 0");
    }

    std::array<char, SERIALIZE_BUFFER_SIZE> temp;
    std::to_chars_result result = std::to_chars(temp.begin(), temp.end(), len);
    if (result.ec == std::errc::value_too_large) {
        throw std::runtime_error("http_output set_content_len: value too large");
    }

    return header("content-length", std::string_view { temp.begin(), result.ptr });
}

cppevent::http_output& cppevent::http_output::chunked() {
    m_chunked_encoding = true;
    return header("transfer-encoding", "chunked");
}

std::string get_status_line(cppevent::HTTP_STATUS status) {
    return std::string { "HTTP/1.1 " }
            .append(cppevent::get_status_reason_phrase(status))
            .append(CRLF_SEPARATOR);
}

cppevent::task<> cppevent::http_output::write(const void* src, long size) {
    if (!m_headers_written) {
        co_await raw_write(get_status_line(m_status));
        co_await raw_write(m_headers_buf);
        m_headers_written = true;
    }
    
    if (m_chunked_encoding && size >= 0) {
        std::array<char, SERIALIZE_BUFFER_SIZE> temp;
        std::to_chars_result result = std::to_chars(temp.begin(), temp.end(), size, HEX_BASE);
        if (result.ec == std::errc::value_too_large) {
            throw std::runtime_error("http_output write: size too large");
        }
        co_await raw_write(temp.begin(), result.ptr - temp.begin());
        co_await raw_write(CRLF_SEPARATOR);
        co_await raw_write(src, size);
        co_await raw_write(CRLF_SEPARATOR);
    } else if (size > 0) {
        co_await raw_write(src, size);
    }

    co_await m_sock.flush();
}

cppevent::task<> cppevent::http_output::write(std::string_view sv) {
    return write(sv.data(), sv.size());
}
