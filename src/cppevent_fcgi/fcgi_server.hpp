#ifndef CPPEVENT_FCGI_FCGI_SERVER_HPP
#define CPPEVENT_FCGI_FCGI_SERVER_HPP

#include "types.hpp"
#include "fcgi_handler.hpp"
#include "request.hpp"

#include <cppevent_net/server.hpp>

#include <cppevent_base/task.hpp>

#include <string>
#include <memory>

namespace cppevent {

using request_ptr = std::unique_ptr<request>;

class event_loop;

class router;

class fcgi_server : public connection_handler {
private:
    event_loop& m_loop;
    fcgi_handler m_handler;
    server m_server;

    awaitable_task<void> write_res(socket& sock, output_queue& out_queue);
    awaitable_task<void> read_req(socket& sock, output_queue& out_queue, 
                                  std::unordered_map<int, request_ptr>& requests);
public:
    fcgi_server(const char* name, const char* service, event_loop& loop, router& router);
    fcgi_server(const char* unix_path, event_loop& loop, router& router);

    task on_connection(std::unique_ptr<socket> sock);
};

}

#endif
