#ifndef CPPEVENT_BASE_BUFFER_HPP
#define CPPEVENT_BASE_BUFFER_HPP

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstring>

namespace cppevent {

struct io_chunk {
    std::byte* m_ptr = nullptr;
    long m_size = 0;
};

struct io_chunk_group {
    int m_count = 0;
    std::array<io_chunk, 2> m_chunks = {
        io_chunk {}, 
        io_chunk {}
    };
};

template<long BUFFER_SIZE>
class byte_buffer {
private:
    std::array<std::byte, BUFFER_SIZE> m_buffer;
    long m_start = 0;
    long m_end = 0;

    std::byte* get_ptr(long index) {
        return m_buffer.data() + (index % BUFFER_SIZE);
    }

public:
    long available() {
        return m_end - m_start;
    }

    long capacity() {
        return BUFFER_SIZE - available();
    }

    void increment_read_p(long size) {
        m_start += size;
        if (m_start > BUFFER_SIZE && m_end > BUFFER_SIZE) {
            m_start -= BUFFER_SIZE;
            m_end -= BUFFER_SIZE;
        }
    }

    void increment_write_p(long size) {
        m_end += size;
    }

    io_chunk get_read_chunk() {
        long boundary = (m_start / BUFFER_SIZE + 1) * BUFFER_SIZE;
        long max_size = std::min(boundary, m_end) - m_start;
        return { get_ptr(m_start) , max_size };
    }

    io_chunk_group get_read_chunks() {
        long boundary = (m_start / BUFFER_SIZE + 1) * BUFFER_SIZE;
        if (boundary >= m_end) {
            return io_chunk_group {
                m_end > m_start,
                std::array<io_chunk, 2> {
                    io_chunk { get_ptr(m_start), m_end - m_start },
                    io_chunk { nullptr, 0 }
                }
            };
        }
        return io_chunk_group {
            2,
            std::array<io_chunk, 2> {
                io_chunk { get_ptr(m_start), boundary - m_start },
                io_chunk { m_buffer.data(), m_end - boundary } 
            }
        };
    }

    io_chunk get_write_chunk() {
        long boundary = (m_end / BUFFER_SIZE + 1) * BUFFER_SIZE;
        long max_size = std::min(boundary, m_start + BUFFER_SIZE) - m_end;
        return { get_ptr(m_end) , max_size };
    }

    io_chunk_group get_write_chunks() {
        long boundary = (m_end / BUFFER_SIZE + 1) * BUFFER_SIZE;
        long write_end = m_start + BUFFER_SIZE;
        if (boundary >= write_end) {
            return io_chunk_group {
                write_end > m_end,
                std::array<io_chunk, 2> {
                    io_chunk { get_ptr(m_end), write_end - m_end },
                    io_chunk { nullptr, 0 }
                }
            };
        }
        return io_chunk_group {
            2,
            std::array<io_chunk, 2> {
                io_chunk { get_ptr(m_end), boundary - m_end },
                io_chunk { m_buffer.data(), write_end - boundary } 
            }
        };
    }

    long read(std::byte* dest, long size) {
        long size_read = 0;
        auto read_group = get_read_chunks();
        for (int i = 0; i < read_group.m_count && size > 0; ++i) {
            io_chunk chunk = read_group.m_chunks[i];
            long size_to_read = std::min(chunk.m_size, size);
            std::memcpy(dest, chunk.m_ptr, size_to_read);
            dest += size_to_read;
            size -= size_to_read;
            size_read += size_to_read;
        }
        increment_read_p(size_read);
        return size_read;
    }

    long read(void* dest, long size) {
        return read(static_cast<std::byte*>(dest), size);
    }

    long write(const std::byte* src, long size) {
        long size_written = 0;
        auto write_group = get_write_chunks();
        for (int i = 0; i < write_group.m_count && size > 0; ++i) {
            io_chunk chunk = write_group.m_chunks[i];
            long size_to_write = std::min(chunk.m_size, size);
            std::memcpy(chunk.m_ptr, src, size_to_write);
            src += size_to_write;
            size -= size_to_write;
            size_written += size_to_write;
        }
        increment_write_p(size_written);
        return size_written;
    }

    long write(const void* src, long size) {
        return write(static_cast<const std::byte*>(src), size);
    }
};

template<long BUFFER_SIZE>
inline bool can_read_buffer(io_chunk& chunk, byte_buffer<BUFFER_SIZE>& buffer) {
    chunk = buffer.get_read_chunk();
    return chunk.m_size > 0;
}

template<long BUFFER_SIZE>
inline bool can_read_buffer(io_chunk_group& group, byte_buffer<BUFFER_SIZE>& buffer) {
    group = buffer.get_read_chunks();
    return group.m_count > 0;
}

template<long BUFFER_SIZE>
inline bool can_write_buffer(io_chunk& chunk, byte_buffer<BUFFER_SIZE>& buffer) {
    chunk = buffer.get_write_chunk();
    return chunk.m_size > 0;
}

template<long BUFFER_SIZE>
inline bool can_write_buffer(io_chunk_group& group, byte_buffer<BUFFER_SIZE>& buffer) {
    group = buffer.get_write_chunks();
    return group.m_count > 0;
}

}

#endif
