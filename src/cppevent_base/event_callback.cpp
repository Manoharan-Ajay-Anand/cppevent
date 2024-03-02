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

cppevent::status_awaiter cppevent::event_callback::await_status() {
    m_store.prepare();
    set_handler([this](e_status status) {
        m_store.resume(status);
    });
    return { &m_store };
}
