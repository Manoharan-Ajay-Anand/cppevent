#include "event_bus.hpp"

#include "status_store.hpp"

#include <utility>

cppevent::event_callback cppevent::event_bus::get_event_callback() {
    status_store* store =  nullptr;
    if (m_released.empty()) {
        e_id id { static_cast<uint32_t>(m_stores.size()), 0 };
        auto ptr = std::make_unique<status_store>(id, m_released);
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
    auto& store = m_stores[id.m_index];
    if (store->get_id() == id) {
        store->notify(status);
    }
}
