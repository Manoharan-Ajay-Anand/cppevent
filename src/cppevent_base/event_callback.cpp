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
    }
}

cppevent::status_store::status_store(event_callback& callback) {
    m_handle = std::noop_coroutine();
    callback.set_handler([this](e_status status) {
        m_status_opt = status;
        m_handle.resume();
    });
}

cppevent::status_awaiter::status_awaiter(event_callback& callback) {
    m_store = std::make_unique<status_store>(callback);
}

bool cppevent::status_awaiter::await_ready() {
    if (m_store) {
        return (m_store->m_status_opt).has_value();
    }
    return true;
}

void cppevent::status_awaiter::await_suspend(std::coroutine_handle<> handle) {
    m_store->m_handle = handle;
}

cppevent::e_status cppevent::status_awaiter::await_resume() {
    if (m_store) {
        return (m_store->m_status_opt).value();
    }
    return 0;
}
