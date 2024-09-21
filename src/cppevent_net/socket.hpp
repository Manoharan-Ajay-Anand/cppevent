#ifndef CPPEVENT_NET_SOCKET_HPP
#define CPPEVENT_NET_SOCKET_HPP

#include "types.hpp"

#include <cppevent_base/byte_buffer.hpp>
#include <cppevent_base/task.hpp>
#include <cppevent_base/types.hpp>

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

    task<e_status> recv_incoming();
public:
    socket(int fd, std::unique_ptr<io_listener>&& read_l,
           std::unique_ptr<io_listener>&& write_l);
    socket(int fd, event_loop& loop);
    ~socket();

    void shutdown();

    task<std::span<std::byte>> peek();
    void seek(long offset);

    task<long> read(void* dest, long size, bool read_fully);
    task<long> read(std::string& dest, long size, bool read_fully);
    task<int> read_c(bool read_fully);
    task<long> skip(long size, bool skip_fully);

    task<> write(const void* src, long size);
    task<> flush();
};

}

#endif
