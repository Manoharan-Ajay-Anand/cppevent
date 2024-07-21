#include "http_body.hpp"

#include "http_line.hpp"

#include <cppevent_net/socket.hpp>

#include <charconv>
#include <algorithm>

cppevent::http_body::http_body(long incoming, bool ended, socket& sock): m_incoming(incoming),
                                                                         m_ended(ended),
                                                                         m_sock(sock) {    
}

cppevent::awaitable_task<bool> cppevent::http_body::has_incoming() {
    if (m_incoming > 0 || m_ended) {
        co_return m_incoming > 0;
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
        m_incoming = chunk_len;
    }

    if (m_incoming == 0) m_ended = true;

    co_return m_incoming > 0;
}

cppevent::awaitable_task<long> cppevent::http_body::read(void* dest, long size) {
    long total_read = 0;
    
    while (total_read < size && co_await has_incoming()) {
        long to_read = std::min(size - total_read, m_incoming);
        co_await m_sock.read(dest, to_read, true);
        total_read += to_read;
    }

    co_return total_read;
}

cppevent::awaitable_task<long> cppevent::http_body::read(std::string& dest, long size) {
    long total_read = 0;
    
    while (total_read < size && co_await has_incoming()) {
        long to_read = std::min(size - total_read, m_incoming);
        co_await m_sock.read(dest, to_read, true);
        total_read += to_read;
    }

    co_return total_read;
}

cppevent::awaitable_task<long> cppevent::http_body::skip(long size) {
    long total_skipped = 0;
    
    while (total_skipped < size && co_await has_incoming()) {
        long to_skip = std::min(size - total_skipped, m_incoming);
        co_await m_sock.skip(to_skip, true);
        total_skipped += to_skip;
    }

    co_return total_skipped;
}
