#include "types.hpp"

char lowercase(char c) {
    if (c >= 'A' && c <= 'Z') {
        return c + 32;
    }
    return c;
}

size_t cppevent::case_insensitive_hash::operator()(const std::string_view& key) const{
    size_t seed = 0;
    std::hash<char> hasher;
    for (char c : key) {
        seed ^= hasher(lowercase(c)) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    }
    return seed;
}

bool cppevent::case_insensitive_equality::operator()(const std::string_view& a,
                                                     const std::string_view& b) const{
    if (a.size() != b.size()) {
        return false;
    }
    for (long i = 0; i < a.size(); ++i) {
        if (lowercase(a[i]) != lowercase(b[i])) {
            return false;
        }
    }
    return true;
}
