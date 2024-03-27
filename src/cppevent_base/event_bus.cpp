#include "event_bus.hpp"

#include "event_callback.hpp"

#include <utility>

cppevent::e_id cppevent::event_bus::register_event_callback(event_callback* callback) {
    e_id id = ++m_id_counter;
    m_callbacks[id] = callback;
    return id;
}

void cppevent::event_bus::deregister_event_callback(e_id id) {
    m_to_remove.push(id);
}

void cppevent::event_bus::notify(e_id id, e_status status) {
    while (!m_to_remove.empty()) {
        m_callbacks.erase(m_to_remove.top());
        m_to_remove.pop();
    }
    auto it = m_callbacks.find(id);
    if (it != m_callbacks.end()) {
        (it->second)->notify(status);
    }
}
