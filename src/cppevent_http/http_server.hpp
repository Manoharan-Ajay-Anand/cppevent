#ifndef CPPEVENT_HTTP_HTTP_SERVER_HPP
#define CPPEVENT_HTTP_HTTP_SERVER_HPP

#include <cppevent_net/server.hpp>

namespace cppevent {

class http_router;

class http_server : public connection_handler {
private:
    server m_serv;
    http_router& m_router;
public:
    http_server(const char* name, const char* service, http_router& router, event_loop& loop);
    http_server(const char* unix_path, http_router& router, event_loop& loop);

    task<> on_connection(std::unique_ptr<socket> sock);
};

}

#endif
