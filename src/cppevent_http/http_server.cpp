#include "http_server.hpp"

#include "util.hpp"

#include <cppevent_net/socket.hpp>

cppevent::http_server::http_server(const char* name,
                                   const char* service,
                                   event_loop& loop): m_serv(name, service, loop, *this) {
}

cppevent::http_server::http_server(const char* unix_path,
                                   event_loop& loop): m_serv(unix_path, loop, *this) {
}

cppevent::task cppevent::http_server::on_connection(std::unique_ptr<socket> sock) {
    std::vector<std::string> req_header_lines;
    http_line req_line = co_await read_http_line(*sock);
}
