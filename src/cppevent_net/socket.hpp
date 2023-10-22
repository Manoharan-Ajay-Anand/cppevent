#ifndef CPPEVENT_NET_SOCKET_HPP
#define CPPEVENT_NET_SOCKET_HPP

#include "types.hpp"

#include <cppevent_base/byte_buffer.hpp>
#include <cppevent_base/task.hpp>

#include <string>
#include <memory>

namespace cppevent {

class event_loop;

class io_listener;

class socket {
private:
    const int m_fd;
    std::unique_ptr<io_listener> m_read_listener;
    std::unique_ptr<io_listener> m_write_listener;
    byte_buffer<SOCKET_BUFFER_SIZE> m_in_buffer;
    byte_buffer<SOCKET_BUFFER_SIZE> m_out_buffer;
    OP_STATUS m_status;

public:
    socket(int fd, std::unique_ptr<io_listener>&& read_l,
           std::unique_ptr<io_listener>&& write_l);
    socket(int fd, event_loop& loop);
    ~socket();

    awaitable_task<long> read(void* dest, long size, bool read_fully);
    awaitable_task<std::string> read_line(bool read_fully);
    awaitable_task<long> skip(long size, bool skip_fully);

    awaitable_task<void> write(const void* src, long size);
    awaitable_task<void> flush();
};

}

#endif
