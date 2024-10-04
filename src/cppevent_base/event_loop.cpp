#include "event_loop.hpp"

#include "io_listener.hpp"
#include "event_callback.hpp"
#include "util.hpp"

#include <array>
#include <queue>

#include <sys/eventfd.h>
#include <unistd.h>

std::unique_ptr<cppevent::io_listener> cppevent::event_loop::get_io_listener(int fd) {
    return m_io_service.get_listener(fd, m_event_bus);
}

cppevent::event_callback cppevent::event_loop::get_event_callback() {
    return event_callback { m_event_bus };
}

void cppevent::event_loop::add_event(e_event ev) {
    m_events.push(ev);
}

void cppevent::event_loop::run() {
    while (m_running) {
        if (!m_events.empty()) {
            auto event = m_events.front();
            m_events.pop();
            m_event_bus.notify(event.m_id, event.m_status);
            continue;
        }
        add_event(m_io_service.poll());
    }
}

void cppevent::event_loop::stop() {
    m_running = false;
}
