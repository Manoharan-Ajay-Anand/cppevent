#include "event_loop.hpp"

#include "io_listener.hpp"
#include "event_callback.hpp"
#include "util.hpp"

#include <array>
#include <queue>

std::unique_ptr<cppevent::io_listener> cppevent::event_loop::get_io_listener(int fd) {
    return m_io_service.get_listener(fd, m_event_bus);
}

cppevent::event_callback cppevent::event_loop::get_event_callback() {
    return m_event_bus.get_event_callback();
}

void cppevent::event_loop::add_event(e_event ev) {
    m_io_service.add_event(ev);
}

void cppevent::event_loop::run() {
    while (m_running) {
        std::queue<e_event> events = m_io_service.poll_events();
        while (m_running && !events.empty()) {
            e_event ev = events.front();
            events.pop();
            m_event_bus.notify(ev.m_id, ev.m_status);
        }
    }
}

void cppevent::event_loop::stop() {
    m_running = false;
    m_io_service.interrupt();
}
