#ifndef CPPEVENT_HTTP_UTIL_HTTP
#define CPPEVENT_HTTP_UTIL_HTTP

#include <cppevent_base/task.hpp>

#include <string>
#include <vector>
#include <string_view>
#include <map>

namespace cppevent {

std::vector<std::string_view> split_string(std::string_view s, char separator);

std::multimap<std::string_view, std::string_view> retrieve_params(std::string_view s);

std::string_view trim_string(std::string_view s);

size_t find_case_insensitive(std::string_view text, std::string_view search);

}

#endif
