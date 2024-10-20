#include "event_callback.hpp"

#include "status_store.hpp"

cppevent::event_callback::event_callback(status_store* store): m_store(store) {
}

cppevent::event_callback::~event_callback() {
    m_store->release();
}

cppevent::e_id cppevent::event_callback::get_id() const {
    return m_store->get_id();
}

cppevent::status_awaiter cppevent::event_callback::await_status() {
    return status_awaiter { m_store };
}
