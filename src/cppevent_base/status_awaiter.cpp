#include "status_awaiter.hpp"

#include "status_store.hpp"

#include <stdexcept>

cppevent::status_awaiter::status_awaiter(status_store* store): m_store(store) {
}

cppevent::status_awaiter::~status_awaiter() {
    if (m_store) {
        m_store->reset();
    }
}

cppevent::status_awaiter::status_awaiter(status_awaiter&& other) {
    m_store = other.m_store;
    other.m_store = nullptr;
}

cppevent::status_awaiter& cppevent::status_awaiter::operator=(status_awaiter&& other) {
    m_store = other.m_store;
    other.m_store = nullptr;
    return *this;
}

bool cppevent::status_awaiter::await_ready() {
    if (m_store) {
        return m_store->has_status();
    }
    return true;
}

void cppevent::status_awaiter::await_suspend(std::coroutine_handle<> handle) {
    m_store->set_handle(handle);
}

cppevent::e_status cppevent::status_awaiter::await_resume() {
    e_status result = 0;
    if (m_store) {
        result = m_store->consume_status();
    }
    m_store = nullptr;
    return result;
}
