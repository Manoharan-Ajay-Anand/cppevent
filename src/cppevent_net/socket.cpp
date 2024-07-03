#include "socket.hpp"

#include <cppevent_base/event_loop.hpp>
#include <cppevent_base/io_listener.hpp>
#include <cppevent_base/util.hpp>

#include <algorithm>
#include <stdexcept>

cppevent::socket::socket(int fd, std::unique_ptr<io_listener>&& read_l,
                         std::unique_ptr<io_listener>&& write_l): m_fd(fd) {
    m_read_listener = std::move(read_l);
    m_write_listener = std::move(write_l);
}

cppevent::socket::socket(int fd, event_loop& loop): socket(fd,
                                                           loop.get_io_listener(fd),
                                                           loop.get_io_listener(fd)) {
}

cppevent::socket::~socket() {
    int status = ::close(m_fd);
    throw_if_error(status, "Failed to close socket fd: ");
}

void cppevent::socket::shutdown() {
    int status = ::shutdown(m_fd, SHUT_RDWR);
    throw_if_error(status, "Failed to shutdown socket fd: ");
}

cppevent::awaitable_task<cppevent::e_status> cppevent::socket::recv_incoming() {
    io_chunk chunk = m_in_buffer.get_write_chunk();
    e_status status = co_await m_read_listener->on_recv(chunk.m_ptr, chunk.m_size, 0);
    if (status > 0) m_in_buffer.increment_write_p(status);
    co_return status;
}

cppevent::awaitable_task<cppevent::e_status> cppevent::socket::send_outgoing() {
    io_chunk chunk = m_out_buffer.get_read_chunk();
    e_status status = co_await m_write_listener->on_send(chunk.m_ptr, chunk.m_size, MSG_NOSIGNAL);
    if (status > 0) m_out_buffer.increment_read_p(status);
    co_return status;
}

cppevent::awaitable_task<long> cppevent::socket::read(void* dest, long size, bool read_fully) {
    std::byte* dest_p = static_cast<std::byte*>(dest);
    long total;

    for (total = m_in_buffer.read(dest_p, size);
         total < size;
         total += m_in_buffer.read(dest_p + total, size - total)) {
        e_status status = co_await recv_incoming();
        if (status < 0) {
            throw_error("socket read failed: ", 0 - status);
        } else if (status == 0) {
            if (read_fully) {
                throw std::runtime_error("socket read failed: socket closed");   
            }
            break;
        }
    }

    co_return total;
}



cppevent::awaitable_task<long> cppevent::socket::read(std::string& dest, long size, bool read_fully) {
    e_status status = 0;
    long total;

    for (total = m_in_buffer.read(dest, size);
         total < size;
         total += m_in_buffer.read(dest, size - total)) {
        e_status status = co_await recv_incoming();
        if (status < 0) {
            throw_error("socket read failed: ", 0 - status);
        } else if (status == 0) {
            if (read_fully) {
                throw std::runtime_error("socket read failed: socket closed");   
            }
            break;
        }
    }
    
    co_return total;
}

cppevent::awaitable_task<int> cppevent::socket::read_c(bool read_fully) {
    if (m_in_buffer.available() == 0) {
        e_status status = co_await recv_incoming();
        if (status < 0) {
            throw_error("socket read_c failed: ", 0 - status);
        } else if (status == 0 && read_fully) {
            throw std::runtime_error("socket read_c failed: socket closed");
        }
    }
    co_return m_in_buffer.read_c();
}

template <long BUFFER_SIZE>
long skip_buffer(cppevent::byte_buffer<BUFFER_SIZE>& buf, long size) {
    long to_skip = std::min(buf.available(), size);
    buf.increment_read_p(to_skip);
    return to_skip;
}

cppevent::awaitable_task<long> cppevent::socket::skip(long size, bool skip_fully) {
    e_status status = 0;
    long total;

    for (total = skip_buffer(m_in_buffer, size);
         total < size;
         total += skip_buffer(m_in_buffer, size - total)) {
        e_status status = co_await recv_incoming();
        if (status < 0) {
            throw_error("socket skip failed: ", 0 - status);
        } else if (status == 0) {
            if (skip_fully) {
                throw std::runtime_error("socket skip failed: socket closed");   
            }
            break;
        }
    }
    
    co_return total;
}

cppevent::awaitable_task<void> cppevent::socket::write(const void* src, long size) {
    const std::byte* src_p = static_cast<const std::byte*>(src);
    long total;

    for (total = m_out_buffer.write(src_p, size);
         total < size;
         total += m_out_buffer.write(src_p + total, size - total)) {
        e_status status = co_await send_outgoing();
        if (status < 0) {
            throw_error("socket write failed: ", 0 - status);
        }
    }
}

cppevent::awaitable_task<void> cppevent::socket::flush() {
    while (m_out_buffer.available() > 0) {
        e_status status = co_await send_outgoing();
        if (status < 0) {
            throw_error("socket flush failed: ", 0 - status);
        }
    }
}
