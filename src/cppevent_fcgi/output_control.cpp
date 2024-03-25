#include "output_control.hpp"

#include <cppevent_base/event_loop.hpp>

bool cppevent::output_control_awaiter::await_ready() {
    return m_available;
}

void cppevent::output_control_awaiter::await_suspend(std::coroutine_handle<> handle) {
    m_waiting.push(handle);
}

void cppevent::output_control_awaiter::await_resume() {
    m_available = false;
}

cppevent::output_control::output_control(event_loop& loop): m_loop(loop) {
}

cppevent::output_control_awaiter cppevent::output_control::lock() {
    return { m_available, m_waiting };
}

bool cppevent::output_control::has_pending() const {
    return !m_waiting.empty();
}

void cppevent::output_control::release() {
    m_available = true;
    if (has_pending()) {
        auto handle = m_waiting.front();
        m_waiting.pop();
        m_loop.add_op([handle]() { handle.resume(); });
    }
}
