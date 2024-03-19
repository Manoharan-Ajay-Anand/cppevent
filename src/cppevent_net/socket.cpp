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
    int status = close(m_fd);
    throw_if_error(status, "Failed to close socket fd: ");
}

cppevent::awaitable_task<long> cppevent::socket::read(void* dest, long size, bool read_fully) {
    std::byte* dest_p = static_cast<std::byte*>(dest);
    e_status status;
    long total = 0;

    do {
        long transferred = m_in_buffer.read(dest_p, size);
        dest_p += transferred;
        size -= transferred;
        total += transferred;
        if (size > 0) {
            status = co_await read_io_to_buf(*m_read_listener, m_in_buffer);
        }
    } while (size > 0 && status > 0);

    if (status < 0) {
        throw_error("socket read failed: ", 0 - status);
    }
    if (read_fully && size > 0) {
        throw std::runtime_error("socket read failed: socket closed");
    }
    
    co_return total;
}

cppevent::awaitable_task<long> cppevent::socket::read(std::string& dest, long size, bool read_fully) {
    e_status status;
    long total = 0;

    do {
        auto chunk = m_in_buffer.get_read_chunk();
        long transferred = std::min(chunk.m_size, size);
        dest.append(reinterpret_cast<char*>(chunk.m_ptr), transferred);
        size -= transferred;
        total += transferred;
        m_in_buffer.increment_read_p(transferred);
        if (size > 0) {
            status = co_await read_io_to_buf(*m_read_listener, m_in_buffer);
        }
    } while (size > 0 && status > 0);

    if (status < 0) {
        throw_error("socket read failed: ", 0 - status);
    }
    if (read_fully && size > 0) {
        throw std::runtime_error("socket read failed: socket closed");
    }
    
    co_return total;
}

cppevent::awaitable_task<int> cppevent::socket::read_c(bool read_fully) {
    int i = m_in_buffer.read_c();

    if (i < 0) {
        e_status status = co_await read_io_to_buf(*m_read_listener, m_in_buffer);
        if (status < 0) {
            throw_error("socket read_c failed: ", 0 - status);
        } else if (status == 0 && read_fully) {
            throw std::runtime_error("socket read_c failed: socket closed");
        }
        i = m_in_buffer.read_c();
    }
    
    co_return i;
}

cppevent::awaitable_task<long> cppevent::socket::skip(long size, bool skip_fully) {
    e_status status;
    long total = 0;

    do {
        long to_skip = std::min(m_in_buffer.available(), size);
        m_in_buffer.increment_read_p(to_skip);
        size -= to_skip;
        total += to_skip;
        if (size > 0) {
            status = co_await read_io_to_buf(*m_read_listener, m_in_buffer);
        }
    } while (size > 0 && status > 0);
    
    if (status < 0) {
        throw_error("socket skip failed: ", 0 - status);
    }
    if (skip_fully && size > 0) {
        throw std::runtime_error("socket skip failed: socket closed");
    }
    
    co_return total;
}

cppevent::awaitable_task<void> cppevent::socket::write(const void* src, long size) {
    const std::byte* src_p = static_cast<const std::byte*>(src);
    e_status status;

    do {
        long transferred = m_out_buffer.write(src_p, size);
        src_p += transferred;
        size -= transferred;
        if (size > 0) {
            status = co_await write_io_from_buf(*m_write_listener, m_out_buffer);
        }
    } while (size > 0 && status > 0);

    if (status < 0) {
        throw_error("socket write failed: ", 0 - status);
    }
}

cppevent::awaitable_task<void> cppevent::socket::flush() {
    while (m_out_buffer.available() > 0) {
        e_status status = co_await write_io_from_buf(*m_write_listener, m_out_buffer);
        if (status < 0) {
            throw_error("socket flush failed: ", 0 - status);
        }
    }
}
