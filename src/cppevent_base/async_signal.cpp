#include "async_signal.hpp"

void cppevent::signal_trigger::activate() const {
    m_loop->add_event({ m_id, 0 });
}

bool cppevent::signal_awaiter::await_ready() {
    return m_triggered;
}

void cppevent::signal_awaiter::await_suspend(std::coroutine_handle<> handle) {
    m_waiting_opt = handle;
}

void cppevent::signal_awaiter::await_resume() {
    m_triggered = false;
}

cppevent::async_signal::async_signal(event_loop& loop): m_loop(loop),
                                                        m_callback(loop.get_event_callback()) {
    init_handler();
}

void cppevent::async_signal::init_handler() {
    m_callback.set_handler([this](e_status status) {
        init_handler();
        m_triggered = true;
        if (m_waiting_opt.has_value()) {
            auto res_handle = m_waiting_opt.value();
            m_waiting_opt.reset();
            res_handle.resume();
        }
    });
}

cppevent::signal_trigger cppevent::async_signal::get_trigger() {
    return { m_callback.get_id(), &m_loop };
}

cppevent::signal_awaiter cppevent::async_signal::await_signal() {
    return { m_triggered, m_waiting_opt };
}
