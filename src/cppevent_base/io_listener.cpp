#include "io_listener.hpp"

#include "event_bus.hpp"
#include "event_callback.hpp"

#include <liburing.h>

cppevent::io_listener::io_listener(event_bus& bus,
                                   io_uring* ring,
                                   int fd): m_callback(bus.get_event_callback()),
                                            m_ring(ring),
                                            m_fd(fd) {
}

io_uring_sqe* cppevent::io_listener::get_sqe() {
    io_uring_sqe* sqe = io_uring_get_sqe(m_ring);
    io_uring_sqe_set_data64(sqe, static_cast<uint64_t>(m_callback.get_id()));
    return sqe;
}

cppevent::status_awaiter cppevent::io_listener::get_status_awaiter() {
    return m_callback.await_status();
}

cppevent::status_awaiter cppevent::io_listener::on_accept(sockaddr* addr, socklen_t* len) {
    io_uring_sqe* sqe = get_sqe();
    io_uring_prep_accept(sqe, m_fd, addr, len, 0);
    return get_status_awaiter();
}

cppevent::status_awaiter cppevent::io_listener::on_connect(const sockaddr* addr, socklen_t len) {
    io_uring_sqe* sqe = get_sqe();
    io_uring_prep_connect(sqe, m_fd, addr, len);
    return get_status_awaiter();
}

cppevent::status_awaiter cppevent::io_listener::on_read(void* dest, long size) {
    io_uring_sqe* sqe = get_sqe();
    io_uring_prep_read(sqe, m_fd, dest, size, 0);
    return get_status_awaiter();
}

cppevent::status_awaiter cppevent::io_listener::on_write(const void* src, long size) {
    io_uring_sqe* sqe = get_sqe();
    io_uring_prep_write(sqe, m_fd, src, size, 0);
    return get_status_awaiter();
}

cppevent::status_awaiter cppevent::io_listener::on_recv(void* dest, long size, int flags) {
    io_uring_sqe* sqe = get_sqe();
    io_uring_prep_recv(sqe, m_fd, dest, size, flags);
    return get_status_awaiter();
}

cppevent::status_awaiter cppevent::io_listener::on_send(const void* src, long size, int flags) {
    io_uring_sqe* sqe = get_sqe();
    io_uring_prep_send(sqe, m_fd, src, size, flags);
    return get_status_awaiter();
}
    
cppevent::status_awaiter cppevent::io_listener::on_readv(const iovec *iov, int iovcnt) {
    io_uring_sqe* sqe = get_sqe();
    io_uring_prep_readv(sqe, m_fd, iov, iovcnt, 0);
    return get_status_awaiter();
}

cppevent::status_awaiter cppevent::io_listener::on_writev(const iovec *iov, int iovcnt) {
    io_uring_sqe* sqe = get_sqe();
    io_uring_prep_writev(sqe, m_fd, iov, iovcnt, 0);
    return get_status_awaiter();
}
