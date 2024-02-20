#include "event_callback.hpp"

#include "event_bus.hpp"

#include <stdexcept>

cppevent::event_callback::event_callback(event_bus& bus): m_bus(bus) {
    m_id = m_bus.register_event_callback(this);
}

cppevent::event_callback::~event_callback() {
    m_bus.deregister_event_callback(m_id);
}

cppevent::e_id cppevent::event_callback::get_id() const {
    return m_id;
}

std::optional<cppevent::e_status> cppevent::event_callback::get_status() {
    auto result = m_status_opt;
    m_status_opt.reset();
    return result;
}

void cppevent::event_callback::set_handler(const e_handler& handler) {
    if (m_handler_opt) {
        throw std::runtime_error("event_callback set_handler: handler already set");
    }
    m_handler_opt = handler;
}

void cppevent::event_callback::notify(e_status status) {
    if (m_handler_opt) {
        auto handler = m_handler_opt.value();
        m_handler_opt.reset();
        handler(status);
    } else {
        m_status_opt = status;
    }
}

bool cppevent::status_awaiter::await_ready() {
    auto status_opt = m_callback.get_status();
    if (status_opt) {
        m_status = status_opt.value();
        return true;
    }
    return false;
}

void cppevent::status_awaiter::await_suspend(std::coroutine_handle<> handle) {
    m_callback.set_handler([this, handle](e_status status) {
        m_status = status;
        handle.resume();
    });
}

cppevent::e_status cppevent::status_awaiter::await_resume() {
    return m_status;
}
