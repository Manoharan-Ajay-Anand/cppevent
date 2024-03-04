#ifndef CPPEVENT_NET_SERVER_SOCKET_HPP
#define CPPEVENT_NET_SERVER_SOCKET_HPP

#include <memory>
#include <string>

#include <cppevent_base/task.hpp>

namespace cppevent {

class io_listener;

class event_loop;

class socket;

class server_socket {
private:
    event_loop& m_loop;
    int m_fd;
    std::unique_ptr<io_listener> m_listener;
public:
    server_socket(const char* name, const char* service, event_loop& loop);
    server_socket(const char* unix_path, event_loop& loop);
    ~server_socket();

    awaitable_task<std::unique_ptr<socket>> accept();
};

}

#endif
