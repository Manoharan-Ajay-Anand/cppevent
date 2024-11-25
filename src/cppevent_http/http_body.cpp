#include "http_body.hpp"

#include "http_line.hpp"
#include "types.hpp"

#include <cppevent_net/socket.hpp>

#include <charconv>
#include <algorithm>
#include <stdexcept>

cppevent::http_body::http_body(long incoming, bool ended, socket& sock): m_incoming(incoming),
                                                                         m_ended(ended),
                                                                         m_sock(sock) {    
}

cppevent::task<bool> cppevent::http_body::has_incoming() {
    if (m_incoming > 0 || m_ended) {
        co_return m_incoming > 0;
    }

    http_line line = co_await read_http_line(m_sock);
    if (line.is_last_line()) {
        line = co_await read_http_line(m_sock);
    }

    if (!line.has_value()) {
        throw std::runtime_error("http_body has_incoming: no chunk");
    }

    long chunk_len = 0;
    std::string_view sv = line.m_val;
    std::from_chars_result result = std::from_chars(sv.begin(), sv.end(), chunk_len, HEX_BASE);
    if (result.ec != std::errc {} || chunk_len < 0) {
        throw std::runtime_error("http_body has_incoming: invalid chunk size");
    }
    m_incoming = chunk_len;

    if (m_incoming == 0) {
        m_ended = true;
        co_await read_http_line(m_sock);
    }

    co_return m_incoming > 0;
}

cppevent::task<long> cppevent::http_body::read(void* dest, long size) {
    long total_read = 0;
    
    while (total_read < size) {
        if (!co_await has_incoming()) break;
        long size_to_read = std::min(size - total_read, m_incoming);
        co_await m_sock.read(dest, size_to_read, true);
        m_incoming -= size_to_read;
        total_read += size_to_read;
    }

    co_return total_read;
}

cppevent::task<long> cppevent::http_body::read(std::string& dest, long size) {
    long total_read = 0;
    
    while (total_read < size) {
        if (!co_await has_incoming()) break;
        long size_to_read = std::min(size - total_read, m_incoming);
        co_await m_sock.read(dest, size_to_read, true);
        m_incoming -= size_to_read;
        total_read += size_to_read;
    }

    co_return total_read;
}

cppevent::task<long> cppevent::http_body::skip(long size) {
    long total_skipped = 0;
    
    while (total_skipped < size) {
        if (!co_await has_incoming()) break;
        long size_to_skip = std::min(size - total_skipped, m_incoming);
        co_await m_sock.skip(size_to_skip, true);
        m_incoming -= size_to_skip;
        total_skipped += size_to_skip;
    }

    co_return total_skipped;
}

cppevent::task<void> cppevent::http_body::await_conn_close() {
    co_await skip(LONG_MAX);
    co_await m_sock.peek();
}
