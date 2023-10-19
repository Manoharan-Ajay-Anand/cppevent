#include "server_socket.hpp"

#include "socket.hpp"
#include "util.hpp"

#include <cppevent_base/event_loop.hpp>
#include <cppevent_base/io_listener.hpp>
#include <cppevent_base/util.hpp>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

cppevent::server_socket::server_socket(const char* name,
                                       const char* service,
                                       event_loop& loop): m_loop(loop) {
    addrinfo* res;
    get_addrinfo(name, service, &res, true);

    m_fd = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    throw_if_error(m_fd, "server_socket failed to create socket: ");

    int status = ::bind(m_fd, res->ai_addr, res->ai_addrlen);
    throw_if_error(status, "server_socket bind failed: ");

    status = ::listen(m_fd, 5);
    throw_if_error(status, "server_socket listen failed: ");

    m_listener = loop.get_io_listener(m_fd);

    ::freeaddrinfo(res);
}

cppevent::server_socket::server_socket(const std::string& name,
                                       const std::string& service,
                                       event_loop& loop): server_socket(name.c_str(),
                                                                        service.c_str(),
                                                                        loop) {
}

cppevent::server_socket::~server_socket() {
    int status = ::close(m_fd);
    throw_if_error(status, "server_socket close failed: ");
}

cppevent::awaitable_task<std::unique_ptr<cppevent::socket>> cppevent::server_socket::accept() {
    sockaddr_storage client_addr {};
    socklen_t client_addr_len = 0;

    e_status status = co_await m_listener->on_accept(reinterpret_cast<sockaddr*>(&client_addr),
                                                     &client_addr_len);
    
    if (status < 0) {
        throw_error("server_socket accept failed: ", 0 - status);
    }
    co_return std::make_unique<socket>(status, m_loop);
}
