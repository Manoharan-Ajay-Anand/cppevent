#ifndef CPPEVENT_FCGI_FCGI_SERVER_HPP
#define CPPEVENT_FCGI_FCGI_SERVER_HPP

#include "fcgi_handler.hpp"
#include "request.hpp"

#include <cppevent_base/task.hpp>
#include <cppevent_base/async_signal.hpp>

#include <cppevent_net/server.hpp>

#include <string>
#include <memory>

namespace cppevent {

class output_control;

class event_loop;

class router;

class fcgi_server : public connection_handler {
private:
    event_loop& m_loop;
    fcgi_handler m_handler;
    server m_server;

    awaitable_task<void> read_req(socket& sock,
                                  output_control& control,
                                  request_map& requests,
                                  signal_trigger trigger);
public:
    fcgi_server(const char* name, const char* service, event_loop& loop, router& router);
    fcgi_server(const char* unix_path, event_loop& loop, router& router);

    task on_connection(std::unique_ptr<socket> sock);
};

}

#endif
