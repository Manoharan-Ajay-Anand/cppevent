#include "server.hpp"

#include "socket.hpp"

#include <cppevent_base/event_loop.hpp>

cppevent::server::server(const char* name,
                         const char* service,
                         event_loop& loop,
                         connection_handler& handler): m_socket(name, service, loop), 
                                                       m_handler(handler) {
    accept_connections();
}

cppevent::server::server(const std::string& name,
                         const std::string& service,
                         event_loop& loop,
                         connection_handler& handler): server(name.c_str(),
                                                              service.c_str(),
                                                              loop,
                                                              handler) {

}

cppevent::task cppevent::server::accept_connections() {
    while (true) {
        m_handler.on_connection(co_await m_socket.accept());
    }
}
