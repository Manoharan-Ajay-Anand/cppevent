#include "async_signal.hpp"

cppevent::signal_trigger::signal_trigger(e_id id, event_loop* loop): m_id(id),
                                                                     m_loop(loop) {}

void cppevent::signal_trigger::notify() const {
    m_loop->add_event({ m_id, 0 });
}

cppevent::async_signal::async_signal(event_loop& loop): m_loop(loop),
                                                        m_callback(m_loop.get_event_callback()) {
}

cppevent::signal_trigger cppevent::async_signal::get_trigger() {
    return { m_callback.get_id(), &m_loop };
}

cppevent::status_awaiter cppevent::async_signal::await_signal() {
    return m_callback.await_status();
}
