#include "http_body.hpp"

#include "http_line.hpp"

#include <cppevent_net/socket.hpp>

#include <charconv>

cppevent::http_body::http_body(long available, bool ended, socket& sock): m_available(available),
                                                                          m_ended(ended),
                                                                          m_sock(sock) {    
}

cppevent::awaitable_task<long> cppevent::http_body::get_available() {
    if (m_available > 0 || m_ended) {
        co_return m_available;
    }

    http_line line = co_await read_http_line(m_sock);
    if (line.is_last_line()) {
        line = co_await read_http_line(m_sock);
    }

    if (line.has_value()) {
        long chunk_len = 0;
        std::string_view sv = line.m_val;
        std::from_chars_result result = std::from_chars(sv.begin(), sv.end(), chunk_len, 16);
        if (result.ec == std::errc {} && chunk_len == 0) {
            co_await read_http_line(m_sock);
        }
        m_available = chunk_len;
    }

    if (m_available == 0) m_ended = true;

    co_return m_available;
}
