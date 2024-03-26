#include "output_control.hpp"

#include "record.hpp"
#include "request.hpp"

#include <cppevent_net/socket.hpp>

#include <stdexcept>

bool cppevent::output_task_awaiter::await_ready() {
    return !m_out_records.empty();
}

void cppevent::output_task_awaiter::await_suspend(std::coroutine_handle<> handle) {
    m_output_handle_opt = handle;
}

void cppevent::output_task_awaiter::await_resume() {
}

bool cppevent::fcgi_write_awaiter::await_ready() {
    return m_shutdown;
}

std::coroutine_handle<> cppevent::fcgi_write_awaiter::await_suspend(std::coroutine_handle<> handle) {
    m_out_records.push({ m_type, m_req_id, m_src, m_size });
    m_waiting_out_opt = handle;
    std::coroutine_handle<> res_handle = std::noop_coroutine();
    if (m_output_handle_opt.has_value()) {
        res_handle = m_output_handle_opt.value();
        m_output_handle_opt.reset();
    }
    return res_handle;
}

void cppevent::fcgi_write_awaiter::await_resume() {
    if (m_shutdown) {
        throw std::runtime_error("FCGI socket write failed");
    }
}

cppevent::output_control::output_control(request_map& req_map): m_req_map(req_map) {
}

constexpr uint8_t PADDING_DATA[cppevent::FCGI_MAX_PADDING] = {};

cppevent::awaitable_task<void> cppevent::output_control::begin_res_task(socket& sock) {
    while (!m_shutdown || !m_out_records.empty()) {
        co_await output_task_awaiter { m_out_records, m_output_handle_opt };
        if (m_out_records.empty()) {
            break;
        }
        bool to_flush = m_out_records.size() == 1;
        output_record& o = m_out_records.front();
        if (!m_shutdown) {
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
                m_shutdown = true;
            }
        }
        auto it = m_req_map.find(o.m_req_id);
        if (it != m_req_map.end()) {
            (it->second)->resume_output();
        }
        m_out_records.pop();
    }
}

cppevent::fcgi_write_awaiter cppevent::output_control::write(long m_type,
                                                             long m_req_id,
                                                             const void* m_src, long m_size,
                                                             coroutine_opt& m_waiting_out_opt) {
    return { 
        m_type, m_req_id, m_src, m_size,
        m_waiting_out_opt, m_out_records, m_output_handle_opt, m_shutdown
    };
}

void cppevent::output_control::shutdown() {
    m_shutdown = true;
    if (m_output_handle_opt.has_value()) {
        auto res_handle = m_output_handle_opt.value();
        m_output_handle_opt.reset();
        res_handle.resume();
    }
}
