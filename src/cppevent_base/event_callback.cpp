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

void cppevent::event_callback::set_handle(std::coroutine_handle<> handle) {
    if (m_handle_opt) {
        throw std::runtime_error("event_callback set_handle: handle already set");
    }
    m_handle_opt = handle;
}

void cppevent::event_callback::notify(e_status status) {
    m_status_opt = status;
    std::coroutine_handle<> res_handle = m_handle_opt.value_or(std::noop_coroutine());
    m_handle_opt.reset();
    res_handle.resume();
}

bool cppevent::event_callback::has_status() const {
    return m_status_opt.has_value();
}

cppevent::e_status cppevent::event_callback::get_status() {
    return m_status_opt.value();
}

void cppevent::event_callback::reset() {
    m_handle_opt.reset();
    m_status_opt.reset();
}

cppevent::status_awaiter cppevent::event_callback::await_status() {
    return { this };
}
