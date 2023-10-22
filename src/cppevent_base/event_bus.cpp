#include "event_bus.hpp"

#include <utility>

cppevent::event_callback* cppevent::event_bus::get_event_callback() {
    e_id id = ++m_id_counter;
    auto p = m_callbacks.try_emplace(id, id, *this);
    return &(p.first->second);
}

void cppevent::event_bus::remove_event_callback(e_id id) {
    m_marked_deletion.push(id);
}

void cppevent::event_bus::notify(e_id id, e_status status) {
    auto it = m_callbacks.find(id);
    if (it != m_callbacks.end()) {
        (it->second).notify(status);
    }
    while (!m_marked_deletion.empty()) {
        m_callbacks.erase(m_marked_deletion.front());
        m_marked_deletion.pop();
    }
}
