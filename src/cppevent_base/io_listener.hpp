#ifndef CPPEVENT_BASE_IO_LISTENER_HPP
#define CPPEVENT_BASE_IO_LISTENER_HPP

#include "types.hpp"
#include "event_callback.hpp"

#include <sys/socket.h>

struct io_uring;

struct io_uring_sqe;

namespace cppevent {

class io_listener {
private:
    event_callback m_callback;
    io_uring* const m_ring;
    const int m_fd;

    io_uring_sqe* get_sqe();
    status_awaiter get_status_awaiter();

public:
    io_listener(event_bus& bus, io_uring* ring, int fd);

    io_listener(const io_listener&) = delete;
    io_listener& operator=(const io_listener&) = delete;

    io_listener(io_listener&&) = delete;
    io_listener& operator=(io_listener&&) = delete;

    status_awaiter on_accept(sockaddr* addr, socklen_t* len);
    status_awaiter on_connect(const sockaddr* addr, socklen_t len);

    status_awaiter on_read(void* dest, long size);
    status_awaiter on_write(const void* src, long size);

    status_awaiter on_recv(void* dest, long size, int flags);
    status_awaiter on_send(const void* src, long size, int flags);

    status_awaiter on_readv(const iovec *iov, int iovcnt);
    status_awaiter on_writev(const iovec *iov, int iovcnt);
};

}

#endif
