#include "suspended_coro.hpp"

bool cppevent::suspended_coro::has_handle() const {
    return m_stored;
}

void cppevent::suspended_coro::store_handle(const std::coroutine_handle<>& handle) {
    m_handle = handle;
    m_stored = true;
}

std::coroutine_handle<> cppevent::suspended_coro::retrieve_handle() {
    std::coroutine_handle<> retrieved = m_handle;
    reset();
    return retrieved;
}

void cppevent::suspended_coro::reset() {
    m_handle = std::noop_coroutine();
    m_stored = false;
}
