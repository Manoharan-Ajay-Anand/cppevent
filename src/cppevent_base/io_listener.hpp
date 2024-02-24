#ifndef CPPEVENT_BASE_IO_LISTENER_HPP
#define CPPEVENT_BASE_IO_LISTENER_HPP

#include "types.hpp"
#include "event_callback.hpp"
#include "byte_buffer.hpp"
#include "task.hpp"

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

    status_awaiter on_readv(const iovec *iov, int iovcnt);
    status_awaiter on_writev(const iovec *iov, int iovcnt);
};

template<long BUFFER_SIZE>
awaitable_task<e_status> read_io_to_buf(io_listener& listener, byte_buffer<BUFFER_SIZE>& buf) {
    io_chunk_group group = buf.get_write_chunks();
    std::array<iovec, 2> arr = {
        iovec { group.m_chunks[0].m_ptr, static_cast<size_t>(group.m_chunks[0].m_size) },
        iovec { group.m_chunks[1].m_ptr, static_cast<size_t>(group.m_chunks[1].m_size) }
    };
    e_status status = co_await listener.on_readv(arr.data(), group.m_count);
    if (status > 0) {
        buf.increment_write_p(status);
    }
    co_return status;
}

template<long BUFFER_SIZE>
awaitable_task<e_status> write_io_from_buf(io_listener& listener, byte_buffer<BUFFER_SIZE>& buf) {
    io_chunk_group group = buf.get_read_chunks();
    std::array<iovec, 2> arr = {
        iovec { group.m_chunks[0].m_ptr, static_cast<size_t>(group.m_chunks[0].m_size) },
        iovec { group.m_chunks[1].m_ptr, static_cast<size_t>(group.m_chunks[1].m_size) }
    };
    e_status status = co_await listener.on_writev(arr.data(), group.m_count);
    if (status > 0) {
        buf.increment_read_p(status);
    }
    co_return status;
}

}

#endif
