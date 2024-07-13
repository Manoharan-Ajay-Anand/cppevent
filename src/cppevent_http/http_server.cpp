#include "http_server.hpp"

#include "util.hpp"
#include "http_line.hpp"
#include "http_request.hpp"

#include <cppevent_net/socket.hpp>

#include <vector>

cppevent::http_server::http_server(const char* name,
                                   const char* service,
                                   event_loop& loop): m_serv(name, service, loop, *this) {
}

cppevent::http_server::http_server(const char* unix_path,
                                   event_loop& loop): m_serv(unix_path, loop, *this) {
}

cppevent::task cppevent::http_server::on_connection(std::unique_ptr<socket> sock) {
    bool keep_conn = true;
    while (keep_conn) {
        http_request req;
        
        {
            http_line req_line = co_await read_http_line(*sock);
            if (!req_line.has_value() || !req.process_req_line(req_line.m_val)) break;
        }

        {
            http_line header_line;
            for (header_line = co_await read_http_line(*sock);
                header_line.has_value(); 
                header_line = co_await read_http_line(*sock)) {
                if (!req.process_header_line(header_line.m_val)) break;
            }
            if (!header_line.is_last_line()) break;
        }
    }
    sock->shutdown();
}
