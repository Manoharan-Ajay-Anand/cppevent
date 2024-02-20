#include "event_loop.hpp"

#include "io_listener.hpp"
#include "event_callback.hpp"
#include "util.hpp"

#include <array>
#include <queue>

#include <sys/eventfd.h>
#include <unistd.h>

cppevent::event_loop::event_loop() {
    m_event_fd_triggered = false;
    m_running = true;
    m_event_fd = eventfd(0, 0);
    throw_if_error(m_event_fd, "Failed to create event fd: ");
}

cppevent::event_loop::~event_loop() {
    int status = close(m_event_fd);
    throw_if_error(status, "Failed to destroy event fd: ");
}

std::unique_ptr<cppevent::io_listener> cppevent::event_loop::get_io_listener(int fd) {
    return m_io_service.get_listener(fd, m_event_bus);
}

cppevent::event_callback cppevent::event_loop::get_event_callback() {
    return event_callback { m_event_bus };
}

void cppevent::event_loop::trigger_event_fd() {
    if (!m_event_fd_triggered) {
        int status = eventfd_write(m_event_fd, 1);
        throw_if_error(status, "Failed to write to eventfd: ");
        m_event_fd_triggered = true;
    }
}

void cppevent::event_loop::add_event(e_event ev) {
    m_events.push(ev);
    trigger_event_fd();
}

void cppevent::event_loop::add_op(const std::function<void()>& op) {
    m_ops.push(op);
    trigger_event_fd();
}

void cppevent::event_loop::run_ops() {
    std::queue<std::function<void()>> ops = std::move(m_ops);
    while (!ops.empty() && m_running) {
        auto op = ops.front();
        ops.pop();
        op();
    }
}

void cppevent::event_loop::notify_events() {
    std::queue<e_event> events = std::move(m_events);
    while (!events.empty() && m_running) {
        auto event = events.front();
        events.pop();
        m_event_bus.notify(event.m_id, event.m_status);
    }
}

cppevent::awaitable_task<void> cppevent::event_loop::run_internal_loop() {
    std::unique_ptr<io_listener> listener = get_io_listener(m_event_fd);
    uint64_t count;
    while (m_running) {
        int status = co_await listener->on_read(&count, 8);
        if (status >= 0) {
            m_event_fd_triggered = false;
            notify_events();
            run_ops();
        } else {
            throw_error("Failed to read from eventfd: ");
        }
    }
}

void cppevent::event_loop::run() {
    awaitable_task<void> t = run_internal_loop();
    while (m_running) {
        e_event ev = m_io_service.poll();
        m_event_bus.notify(ev.m_id, ev.m_status);
    }
}

void cppevent::event_loop::stop() {
    m_running = false;
}
