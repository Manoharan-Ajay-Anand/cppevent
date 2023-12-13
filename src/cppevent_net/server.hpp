#ifndef CPPEVENT_NET_SERVER_HPP
#define CPPEVENT_NET_SERVER_HPP

#include "server_socket.hpp"

#include <memory>
#include <string>

#include <cppevent_base/task.hpp>

namespace cppevent {

class socket;

class event_loop;

class connection_handler {
public:
    virtual task on_connection(std::unique_ptr<socket> sock) = 0;
};

class server {
private:
    server_socket m_socket;
    connection_handler& m_handler;

    task accept_connections();
public:
    server(const char* name, const char* service, event_loop& loop, connection_handler& handler);
    server(const std::string& name, const std::string& service,
           event_loop& loop, connection_handler& handler);
};

}

#endif
