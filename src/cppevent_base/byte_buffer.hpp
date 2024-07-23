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

    std::byte* read_ptr() {
        return m_buffer.data() + m_start;  
    }

    std::byte* write_ptr() {
        return m_buffer.data() + m_end;
    }

public:
    long available() const {
        return m_end - m_start;
    }

    long capacity() const {
        return BUFFER_SIZE - m_end;
    }

    void increment_read_p(long size) {
        m_start += size;

        if (m_start == m_end) {
            m_start = 0;
            m_end = 0;
        }
    }

    void increment_write_p(long size) {
        m_end += size;
    }

    std::span<std::byte> get_read_chunk() {
        return { read_ptr(), static_cast<size_t>(available()) };
    }

    std::span<std::byte> get_write_chunk() {
        return { write_ptr(), static_cast<size_t>(capacity()) };
    }

    long read(void* dest, long size) {
        long size_to_read = std::min(size, available());
        std::memcpy(dest, read_ptr(), size_to_read);
        increment_read_p(size_to_read);
        return size_to_read;
    }

    long read(std::string& dest, long size) {
        long size_to_read = std::min(size, available());
        dest.append(reinterpret_cast<const char*>(read_ptr()), size_to_read);
        increment_read_p(size_to_read);
        return size_to_read;
    }

    int read_c() {
        if (available() == 0) {
            return -1;
        }
        int i = std::to_integer<int>(*read_ptr());
        increment_read_p(1);
        return i;
    }

    long write(const void* src, long size) {
        long size_to_write = std::min(capacity(), size);
        std::memcpy(write_ptr(), src, size_to_write);
        increment_write_p(size_to_write);
        return size_to_write;
    }
};

}

#endif
