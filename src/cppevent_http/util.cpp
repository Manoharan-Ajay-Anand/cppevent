#include "util.hpp"

#include <cppevent_net/socket.hpp>

std::vector<std::string_view> cppevent::split_string(std::string_view s, char separator) {
    std::vector<std::string_view> result;
    long start = 0;
    for (long i = 0; i <= s.size(); ++i) {
        if (i == s.size() || s[i] == separator) {
            if (start < i) {
                result.push_back(s.substr(start, i - start));
            }
            start = i + 1;
        }
    }
    return result;
}

bool cppevent::http_line::has_value() const {
    return m_received && !m_val.empty();
}

bool cppevent::http_line::is_last_line() const {
    return m_received && m_val.empty();
}

cppevent::awaitable_task<cppevent::http_line> cppevent::read_http_line(socket& sock) {
    std::string line;
    bool line_ended = false;
    while (!line_ended) {
        std::span<std::byte> chunk = co_await sock.peek();
        if (chunk.size() == 0) break;
        
        long offset;
        for (offset = 0; offset < chunk.size() && !line_ended; ++offset) {
            char c = static_cast<char>(chunk[offset]);
            line_ended = c == '\n';
            line.push_back(c);
        }
        sock.seek(offset);
    }
    if (line_ended) {
        line.pop_back();
        if (line.ends_with('\r')) line.pop_back();
    }
    http_line result = { std::move(line), line_ended };
    co_return std::move(result);
}
