#include "stream_awaiters.hpp"

#include <cppevent_base/event_loop.hpp>

bool cppevent::stream_update_awaiter::await_ready() {
    return m_ended;
}

std::coroutine_handle<> cppevent::stream_update_awaiter::await_suspend(std::coroutine_handle<> handle) {
    m_producer = handle;
    auto res_handle = m_consumer.value_or(std::noop_coroutine());
    m_consumer.reset();
    return res_handle;
}

void cppevent::stream_update_awaiter::await_resume() {
    if (m_ended && m_consumer.has_value()) {
        auto res_handle = m_consumer.value();
        m_consumer.reset();
        res_handle.resume();
    }
}

bool cppevent::stream_readable_awaiter::await_ready() {
    return m_remaining > 0 || m_ended;
}

std::coroutine_handle<> cppevent::stream_readable_awaiter::await_suspend(std::coroutine_handle<> handle) {
    m_consumer = handle;
    auto res_handle = m_producer.value_or(std::noop_coroutine());
    m_producer.reset();
    return res_handle;
}

bool cppevent::stream_readable_awaiter::await_resume() {
    return m_remaining > 0;
}
