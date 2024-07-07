#include "http_server.hpp"

#include "util.hpp"

#include <cppevent_net/socket.hpp>

#include <string_view>
#include <iostream>

cppevent::http_server::http_server(const char* name,
                                   const char* service,
                                   event_loop& loop): m_serv(name, service, loop, *this) {
}

cppevent::http_server::http_server(const char* unix_path,
                                   event_loop& loop): m_serv(unix_path, loop, *this) {
}

cppevent::task cppevent::http_server::on_connection(std::unique_ptr<socket> sock) {
    std::vector<std::string> req_header_lines;
    http_line req_line;
    for (req_line = co_await read_http_line(*sock);
         req_line.has_value(); 
         req_line = co_await read_http_line(*sock)) {
        req_header_lines.push_back(std::move(req_line.m_val));
    }
    if (!req_line.m_received) {
        co_return sock->shutdown();
    }
}
