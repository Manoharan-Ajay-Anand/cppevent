#ifndef CPPEVENT_HTTP_HTTP_STRING_HPP
#define CPPEVENT_HTTP_HTTP_STRING_HPP

#include <string_view>

namespace cppevent {

class http_string {
private:
    char* m_data = nullptr;
    long m_size = 0;

public:
    http_string() = default;
    ~http_string();

    http_string(const std::string_view& sv);
    http_string& operator=(const std::string_view& sv);

    http_string(http_string&& other);
    http_string& operator=(http_string&& other);

    std::string_view get_view() const;
};

}

#endif
