#include "status_awaiter.hpp"

#include <stdexcept>

cppevent::awaiter_store::awaiter_store() {
    m_is_set = false;
    m_handle = std::noop_coroutine();
}

void cppevent::awaiter_store::prepare() {
    if (m_is_set) {
        throw std::runtime_error("awaiter store is already set");
    }
    m_is_set = true;
}

void cppevent::awaiter_store::set_handle(std::coroutine_handle<> handle) {
    m_handle = handle;
}

void cppevent::awaiter_store::resume(e_status status) {
    m_status_opt = status;
    m_handle.resume();
}

bool cppevent::awaiter_store::has_status() {
    return m_status_opt.has_value();
}

cppevent::e_status cppevent::awaiter_store::get_status() {
    return m_status_opt.value();
}

void cppevent::awaiter_store::reset() {
    m_is_set = false;
    m_status_opt.reset();
    m_handle = std::noop_coroutine();
}

cppevent::status_awaiter::status_awaiter(awaiter_store* store) {
    m_store = store;
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
    if (m_store) {
        e_status result = m_store->get_status();
        m_store->reset();
        m_store = nullptr;
        return result;
    }
    return 0;
}
