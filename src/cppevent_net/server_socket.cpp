#include "server_socket.hpp"

#include "socket.hpp"
#include "util.hpp"

#include <cppevent_base/event_loop.hpp>
#include <cppevent_base/io_listener.hpp>
#include <cppevent_base/util.hpp>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
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

cppevent::server_socket::server_socket(const char* unix_path,
                                       event_loop& loop): m_loop(loop) {
    if (::access(unix_path, F_OK) == 0) {
        throw_if_error(unlink(unix_path), "server_socket failed to unlink unix path: ");
    }

    m_fd = ::socket(AF_UNIX, SOCK_STREAM, 0);
    throw_if_error(m_fd, "server_socket failed to create socket: ");

    ::sockaddr_un addr {};

    addr.sun_family = AF_UNIX;
    std::strcpy(addr.sun_path, unix_path);

    int status = ::bind(m_fd, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr));
    throw_if_error(status, "server_socket bind failed: ");

    status = ::chmod(unix_path, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    throw_if_error(status, "server_socket chmod failed: ");

    status = ::listen(m_fd, 5);
    throw_if_error(status, "server_socket listen failed: ");

    m_listener = loop.get_io_listener(m_fd);
}

cppevent::server_socket::~server_socket() {
    int status = ::close(m_fd);
    throw_if_error(status, "server_socket close failed: ");
}

cppevent::task<std::unique_ptr<cppevent::socket>> cppevent::server_socket::accept() {
    sockaddr_storage client_addr {};
    socklen_t client_addr_len = 0;

    e_status status = co_await m_listener->on_accept(reinterpret_cast<sockaddr*>(&client_addr),
                                                     &client_addr_len);
    
    if (status < 0) {
        throw_error("server_socket accept failed: ", 0 - status);
    }
    co_return std::make_unique<socket>(status, m_loop);
}
