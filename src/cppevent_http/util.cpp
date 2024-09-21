#include "util.hpp"

#include <cctype>

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
            if (start < i && !key.empty()) {
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

size_t cppevent::find_case_insensitive(std::string_view text, std::string_view search) {
    if (search.empty()) {
        return 0;
    }
    long search_len = search.size();
    long last_pos = text.size() - search_len;
    for (long i = 0; i <= last_pos; ++i) {
        long j;
        for (j = 0; j < search_len && std::tolower(text[i + j]) == std::tolower(search[j]); ++j);
        if (j == search_len) return i;
    }
    return std::string_view::npos;
}
