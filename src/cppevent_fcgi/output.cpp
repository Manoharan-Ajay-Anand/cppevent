#include "output.hpp"

#include "record.hpp"
#include "output_control.hpp"

#include <cppevent_net/socket.hpp>

cppevent::output::output(int req_id, int type,
                         output_control& control,
                         coroutine_opt& wait_out_opt): m_req_id(req_id),
                                                       m_type(type),
                                                       m_control(control),
                                                       m_waiting_out_opt(wait_out_opt) {
}

constexpr uint8_t PADDING_DATA[cppevent::FCGI_MAX_PADDING] = {};

cppevent::fcgi_write_awaiter cppevent::output::write(const void* src, long size) {
    return m_control.write(m_type, m_req_id, src, size, m_waiting_out_opt);
}

cppevent::fcgi_write_awaiter cppevent::output::write(std::string_view s) {
    return write(s.data(), static_cast<long>(s.size()));
}

cppevent::fcgi_write_awaiter cppevent::output::end() {
    return write(nullptr, 0);
}
