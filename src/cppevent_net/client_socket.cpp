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
    m_is_unix_socket = false;
    get_addrinfo(name, service, &m_res, false);                                   
}

cppevent::client_socket::client_socket(const char* unix_path,
                                       event_loop& loop): m_loop(loop) {
    m_is_unix_socket = true;
    m_unix_addr = {};
    m_unix_addr.sun_family = AF_UNIX;
    std::strcpy(m_unix_addr.sun_path, unix_path);
}

cppevent::client_socket::~client_socket() {
    if (!m_is_unix_socket) {
        ::freeaddrinfo(m_res);
    }
}

cppevent::task<std::unique_ptr<cppevent::socket>> cppevent::client_socket::connect() const {
    int fd;
    if (m_is_unix_socket) {
        fd = ::socket(AF_UNIX, SOCK_STREAM, 0);
    } else {
        fd = ::socket(m_res->ai_family, m_res->ai_socktype, m_res->ai_protocol);
    }
    throw_if_error(fd, "client_socket failed to create socket: ");
    
    auto read_listener = m_loop.get_io_listener(fd);
    auto write_listener = m_loop.get_io_listener(fd);

    e_status status;
    if (m_is_unix_socket) {
        status = co_await write_listener->on_connect(reinterpret_cast<const sockaddr*>(&m_unix_addr),
                                                     sizeof(m_unix_addr));
    } else {
        status = co_await write_listener->on_connect(m_res->ai_addr, m_res->ai_addrlen);
    }
    if (status < 0) {
        throw_error("client_socket failed to connect: ", 0 - status);
    }

    co_return std::make_unique<socket>(fd, std::move(read_listener), std::move(write_listener));
}
