#ifndef CPPEVENT_BASE_UTIL_HPP
#define CPPEVENT_BASE_UTIL_HPP

#include <string>
#include <string_view>
#include <cstdint>

#include "byte_buffer.hpp"
#include "io_listener.hpp"
#include "task.hpp"

namespace cppevent {

void throw_error(std::string prefix, int errnum);

void throw_error(std::string_view prefix);

void throw_if_error(int status, std::string_view prefix);

uint16_t read_u16_be(const void* src);

uint32_t read_u32_be(const void* src);

void write_u16_be(void* dest, uint16_t val);

void write_u32_be(void* dest, uint32_t val);

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
