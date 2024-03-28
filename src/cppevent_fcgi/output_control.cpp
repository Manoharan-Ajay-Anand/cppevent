#include "output_control.hpp"

#include "record.hpp"
#include "request.hpp"

#include <cppevent_net/socket.hpp>

#include <stdexcept>

namespace cppevent {

struct output_task_awaiter {
    std::queue<output_record>& m_out_records;
    coroutine_opt& m_output_handle_opt;

    bool await_ready() {
        return !m_out_records.empty();
    }

    void await_suspend(std::coroutine_handle<> handle) {
        m_output_handle_opt = handle;
    }

    void await_resume() {}
};

}

bool cppevent::fcgi_write_awaiter::await_ready() {
    return m_error;
}

std::coroutine_handle<> cppevent::fcgi_write_awaiter::await_suspend(std::coroutine_handle<> handle) {
    m_out_records.push({ m_type, m_req_id, m_src, m_size, handle });
    std::coroutine_handle<> res_handle = std::noop_coroutine();
    if (m_output_handle_opt.has_value()) {
        res_handle = m_output_handle_opt.value();
        m_output_handle_opt.reset();
    }
    return res_handle;
}

void cppevent::fcgi_write_awaiter::await_resume() {
    if (m_error) {
        throw std::runtime_error("FCGI write failed");
    }
}

constexpr uint8_t PADDING_DATA[cppevent::FCGI_MAX_PADDING] = {};

cppevent::awaitable_task<void> cppevent::output_control::begin_res_task(socket& sock) {
    while (true) {
        co_await output_task_awaiter { m_out_records, m_output_handle_opt };
        if (m_out_records.empty()) {
            break;
        }
        bool to_flush = m_out_records.size() == 1;
        output_record& o = m_out_records.front();
        if (!m_error) {
            record r {
                FCGI_VERSION_1,
                static_cast<uint8_t>(o.m_type),
                static_cast<uint16_t>(o.m_req_id),
                static_cast<uint16_t>(o.m_size),
                static_cast<uint8_t>(o.m_size % FCGI_HEADER_LEN)
            };
            uint8_t header_data[FCGI_HEADER_LEN];
            r.serialize(header_data);
            try {
                co_await sock.write(header_data, FCGI_HEADER_LEN);
                co_await sock.write(o.m_src, o.m_size);
                co_await sock.write(PADDING_DATA, r.m_padding_len);
                if (to_flush) co_await sock.flush();
            } catch (...) {
                m_error = true;
            }
        }
        std::coroutine_handle<> res_handle = o.m_handle;
        m_out_records.pop();
        res_handle.resume();
    }
}

cppevent::fcgi_write_awaiter cppevent::output_control::write(long m_type,
                                                             long m_req_id,
                                                             const void* m_src, long m_size) {
    return { 
        m_type, m_req_id, m_src, m_size, m_out_records, m_output_handle_opt, m_error
    };
}
