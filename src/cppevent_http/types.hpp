#ifndef CPPEVENT_HTTP_TYPES_HPP
#define CPPEVENT_HTTP_TYPES_HPP

#include <string_view>
#include <string>
#include <unordered_map>

namespace cppevent {

struct case_insensitive_hash {
    size_t operator()(const std::string_view& key) const;
};

struct case_insensitive_equality {
    bool operator()(const std::string_view& a, const std::string_view& b) const;
};

using header_map = std::unordered_map<std::string_view, std::string,
                                      case_insensitive_hash, case_insensitive_equality>;

}

#endif
