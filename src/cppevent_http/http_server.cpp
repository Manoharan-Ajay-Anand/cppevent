#include "http_server.hpp"

#include <cppevent_net/socket.hpp>

cppevent::http_server::http_server(const char* name,
                                   const char* service,
                                   event_loop& loop): m_serv(name, service, loop, *this) {
}

cppevent::http_server::http_server(const char* unix_path,
                                   event_loop& loop): m_serv(unix_path, loop, *this) {
}

cppevent::task cppevent::http_server::on_connection(std::unique_ptr<socket> sock) {
    std::string req;
    co_await sock->read(req, 3, true);
}
