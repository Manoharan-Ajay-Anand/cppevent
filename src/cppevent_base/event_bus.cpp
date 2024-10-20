#include "event_bus.hpp"

#include "status_store.hpp"

#include <utility>

cppevent::event_callback cppevent::event_bus::get_event_callback() {
    status_store* store =  nullptr;
    if (m_released.empty()) {
        auto ptr = std::make_unique<status_store>(m_stores.size() + 1, m_released);
        store = ptr.get();
        m_stores.push_back(std::move(ptr));    
    } else {
        store = m_released.front();
        m_released.pop();
    }
    store->reset();
    return event_callback { store };
}

void cppevent::event_bus::notify(e_id id, e_status status) {
    m_stores.at(id - 1)->notify(status);
}
