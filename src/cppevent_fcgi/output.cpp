#include "output.hpp"

cppevent::output::output(int req_id,
                         int type,
                         output_queue& out_queue,
                         event_loop& loop): m_req_id(req_id),
                                            m_type(type),
                                            m_out_queue(out_queue),
                                            m_signal(loop) {

}

cppevent::signal_awaiter cppevent::output::write(const void* src, long size) {
    record r {
        FCGI_VERSION_1,
        static_cast<uint8_t>(m_type),
        static_cast<uint16_t>(m_req_id),
        static_cast<uint16_t>(size),
        static_cast<uint8_t>((FCGI_HEADER_LEN + size) % FCGI_HEADER_LEN)
    };
    m_out_queue.push({ false, r, src, size, m_signal.get_trigger() });
    return m_signal.await_signal();
}

cppevent::signal_awaiter cppevent::output::write(std::string_view s) {
    return write(s.data(), static_cast<long>(s.size()));
}

cppevent::signal_awaiter cppevent::output::end() {
    return write(nullptr, 0);
}
