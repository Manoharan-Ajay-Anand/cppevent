#include "event_bus.hpp"

#include "event_callback.hpp"

#include <utility>

cppevent::e_id cppevent::event_bus::register_event_callback(event_callback* callback) {
    e_id id = ++m_id_counter;
    m_callbacks[id] = callback;
    return id;
}

void cppevent::event_bus::deregister_event_callback(e_id id) {
    m_callbacks.erase(id);
}

void cppevent::event_bus::notify(e_id id, e_status status) {
    auto it = m_callbacks.find(id);
    if (it != m_callbacks.end()) {
        (it->second)->notify(status);
    }
}
