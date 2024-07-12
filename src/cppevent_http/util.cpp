#include "util.hpp"

#include <cppevent_net/socket.hpp>

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

std::multimap<std::string_view, std::string_view> cppevent::retrieve_params(std::string_view s) {
    std::multimap<std::string_view, std::string_view> result;
    long start = 0;
    std::string_view key;
    for (long i = 0; i <= s.size(); ++i) {
        if (i == s.size() || s[i] == '&') {
            if (start < i) {
                result.insert(std::pair { key, s.substr(start, i - start) });
            }
            start = i + 1;
        } else if (s[i] == '=') {
            if (start < i) {
                key = s.substr(start, i - start);
            }
            start = i + 1;
        }
    }
    return result;
}

std::string_view cppevent::trim_string(std::string_view s) {
    long start;
    for (start = 0; start < s.size() && s[start] == ' '; ++start);

    if (start == s.size()) return std::string_view {};

    long end;
    for (end = s.size() - 1; end > start && s[end] == ' '; --end);

    return s.substr(start, end - start + 1);
}
