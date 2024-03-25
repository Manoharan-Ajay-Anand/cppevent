#include "output.hpp"

#include "record.hpp"
#include "output_control.hpp"

#include <cppevent_net/socket.hpp>

cppevent::output::output(int req_id, int type,
                         output_control& control, socket& sock): m_req_id(req_id), m_type(type),
                                                                 m_control(control), m_sock(sock) {
}

constexpr uint8_t PADDING_DATA[cppevent::FCGI_MAX_PADDING] = {};

cppevent::awaitable_task<void> cppevent::output::write(const void* src, long size) {
    record r {
        FCGI_VERSION_1,
        static_cast<uint8_t>(m_type),
        static_cast<uint16_t>(m_req_id),
        static_cast<uint16_t>(size),
        static_cast<uint8_t>(size % FCGI_HEADER_LEN)
    };
    uint8_t header_data[FCGI_HEADER_LEN];
    r.serialize(header_data);
    co_await m_control.lock();
    co_await m_sock.write(header_data, FCGI_HEADER_LEN);
    co_await m_sock.write(src, size);
    co_await m_sock.write(PADDING_DATA, r.m_padding_len);
    if (!m_control.has_pending()) {
        co_await m_sock.flush();
    }
    m_control.release();
}

cppevent::awaitable_task<void> cppevent::output::write(std::string_view s) {
    return write(s.data(), static_cast<long>(s.size()));
}

cppevent::awaitable_task<void> cppevent::output::end() {
    return write(nullptr, 0);
}
