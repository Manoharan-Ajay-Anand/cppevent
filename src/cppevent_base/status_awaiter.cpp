#include "status_awaiter.hpp"

#include "event_callback.hpp"

#include <stdexcept>

cppevent::status_awaiter::status_awaiter(event_callback* callback): m_callback(callback) {
}

cppevent::status_awaiter::~status_awaiter() {
    if (m_callback) {
        m_callback->reset();
    }
}

cppevent::status_awaiter::status_awaiter(status_awaiter&& other) {
    m_callback = other.m_callback;
    other.m_callback = nullptr;
}

cppevent::status_awaiter& cppevent::status_awaiter::operator=(status_awaiter&& other) {
    m_callback = other.m_callback;
    other.m_callback = nullptr;
    return *this;
}

bool cppevent::status_awaiter::await_ready() {
    if (m_callback) {
        return m_callback->has_status();
    }
    return true;
}

void cppevent::status_awaiter::await_suspend(std::coroutine_handle<> handle) {
    m_callback->set_handle(handle);
}

cppevent::e_status cppevent::status_awaiter::await_resume() {
    if (m_callback) {
        e_status result = m_callback->get_status();
        m_callback->reset();
        m_callback = nullptr;
        return result;
    }
    return 0;
}
