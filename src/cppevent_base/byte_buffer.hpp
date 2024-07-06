#ifndef CPPEVENT_BASE_BUFFER_HPP
#define CPPEVENT_BASE_BUFFER_HPP

#include <algorithm>
#include <array>
#include <string>
#include <cstddef>
#include <cstring>
#include <span>

namespace cppevent {

template<long BUFFER_SIZE>
class byte_buffer {
private:
    std::array<std::byte, BUFFER_SIZE> m_buffer;
    long m_start = 0;
    long m_end = 0;

    long get_read_offset() const {
        return m_start % BUFFER_SIZE;
    }

    long get_write_offset() const {
        return m_end % BUFFER_SIZE;
    }

public:
    long available() const {
        return m_end - m_start;
    }

    long capacity() const {
        return BUFFER_SIZE - available();
    }

    void increment_read_p(long size) {
        m_start += size;
    }

    void increment_write_p(long size) {
        m_end += size;
    }

    std::span<std::byte> get_read_chunk() {
        long boundary = (m_start / BUFFER_SIZE + 1) * BUFFER_SIZE;
        long max_size = std::min(boundary, m_end) - m_start;
        return { m_buffer.data() + get_read_offset(), static_cast<size_t>(max_size) };
    }

    std::span<std::byte> get_write_chunk() {
        long boundary = (m_end / BUFFER_SIZE + 1) * BUFFER_SIZE;
        long max_size = std::min(boundary, m_start + BUFFER_SIZE) - m_end;
        return { m_buffer.data() + get_write_offset(), static_cast<size_t>(max_size) };
    }

    long read(std::byte* dest, long size) {
        long size_read = 0;
        for (auto chunk = get_read_chunk();
             size_read < size && chunk.size() > 0; chunk = get_read_chunk()) {
            long size_to_read = std::min(static_cast<long>(chunk.size()), size - size_read);
            std::memcpy(dest + size_read, chunk.data(), size_to_read);
            size_read += size_to_read;
            increment_read_p(size_to_read);
        }
        return size_read;
    }

    long read(void* dest, long size) {
        return read(static_cast<std::byte*>(dest), size);
    }

    long read(std::string& dest, long size) {
        long size_read = 0;
        for (auto chunk = get_read_chunk();
             size_read < size && chunk.size() > 0; chunk = get_read_chunk()) {
            long size_to_read = std::min(static_cast<long>(chunk.size()), size - size_read);
            dest.append(reinterpret_cast<char*>(chunk.data()), size_to_read);
            size_read += size_to_read;
            increment_read_p(size_to_read);
        }
        return size_read;
    }

    int read_c() {
        if (available() == 0) {
            return -1;
        }
        int i = std::to_integer<int>(m_buffer[get_read_offset()]);
        increment_read_p(1);
        return i;
    }

    long write(const std::byte* src, long size) {
        long size_written = 0;
        for (auto chunk = get_write_chunk();
             size_written < size && chunk.size() > 0; chunk = get_write_chunk()) {
            long size_to_write = std::min(static_cast<long>(chunk.size()), size - size_written);
            std::memcpy(chunk.data(), src + size_written, size_to_write);
            size_written += size_to_write;
            increment_write_p(size_to_write);
        }
        return size_written;
    }

    long write(const void* src, long size) {
        return write(static_cast<const std::byte*>(src), size);
    }
};

}

#endif
