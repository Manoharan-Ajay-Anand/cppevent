#ifndef CPPEVENT_HTTP_HTTP_OUTPUT_HPP
#define CPPEVENT_HTTP_HTTP_OUTPUT_HPP

#include "types.hpp"
#include "http_string.hpp"

#include <string>
#include <vector>
#include <unordered_map>

namespace cppevent {

class socket;

class http_output {
private:
    HTTP_VERSION m_version;
    socket& m_sock;

    std::string m_status_line;
    std::vector<http_string> m_header_names;
    header_map m_header_lookup;
public:
    http_output(HTTP_VERSION version, socket& sock);

    void set_status(HTTP_STATUS status);
    void set_header(std::string_view name, std::string_view value);
};

}

#endif
