#include "client_socket.hpp"

#include "socket.hpp"
#include "util.hpp"

#include <cppevent_base/util.hpp>
#include <cppevent_base/event_loop.hpp>
#include <cppevent_base/io_listener.hpp>

#include <sys/types.h>
#include <sys/socket.h>

#include <cerrno>
#include <stdexcept>

cppevent::client_socket::client_socket(const char* name,
                                       const char* service,
                                       event_loop& loop): m_loop(loop) {
    get_addrinfo(name, service, &m_res, false);                                   
}

cppevent::client_socket::client_socket(const std::string& name,
                                       const std::string& service,
                                       event_loop& loop): client_socket(name.c_str(),
                                                                        service.c_str(),
                                                                        loop) {
}

cppevent::client_socket::~client_socket() {
    ::freeaddrinfo(m_res);
}

cppevent::awaitable_task<std::unique_ptr<cppevent::socket>> cppevent::client_socket::connect() const {
    int fd = ::socket(m_res->ai_family, m_res->ai_socktype, m_res->ai_protocol);
    throw_if_error(fd, "client_socket failed to create socket: ");
    
    auto read_listener = m_loop.get_io_listener(fd);
    auto write_listener = m_loop.get_io_listener(fd);

    e_status status = co_await write_listener->on_connect(m_res->ai_addr, m_res->ai_addrlen);
    if (status < 0) {
        throw_error("client_socket failed to connect: ", 0 - status);
    }

    co_return std::make_unique<socket>(fd, std::move(read_listener), std::move(write_listener));
}
