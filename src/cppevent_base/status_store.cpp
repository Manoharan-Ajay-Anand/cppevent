#include "status_store.hpp"

cppevent::status_store::status_store(e_id id,
                                     std::queue<status_store*>& released): m_id(id),
                                                                           m_released(released) {
}

cppevent::e_id cppevent::status_store::get_id() const {
    return m_id;
}

void cppevent::status_store::notify(e_status status) {
    m_status_opt = status;
    std::coroutine_handle<> h = m_handle;
    m_handle = std::noop_coroutine();
    h.resume();
}

void cppevent::status_store::set_handle(std::coroutine_handle<> h) {
    m_handle = h;
}

bool cppevent::status_store::has_status() const {
    return m_status_opt.has_value();
}

cppevent::e_status cppevent::status_store::consume_status() {
    e_status result = m_status_opt.value_or(0);
    m_status_opt.reset();
    return result;
}

void cppevent::status_store::reset() {
    m_status_opt.reset();
    m_handle = std::noop_coroutine();
}

void cppevent::status_store::release() {
    reset();
    m_released.push(this);
}
