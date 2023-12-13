#include "timer.hpp"

#include "event_loop.hpp"
#include "io_listener.hpp"
#include "util.hpp"

#include <sys/timerfd.h>
#include <unistd.h>

constexpr std::chrono::seconds ONE_SEC(1);

cppevent::timer::timer(std::chrono::nanoseconds interval, event_loop& loop) {
    m_fd = timerfd_create(CLOCK_REALTIME, 0);
    throw_if_error(m_fd, "Failed to create timer fd: ");
    timespec t_spec;
    t_spec.tv_sec = interval / ONE_SEC;
    t_spec.tv_nsec = (interval % ONE_SEC).count();
    itimerspec i_spec = { t_spec, t_spec };
    int status = timerfd_settime(m_fd, 0, &i_spec, NULL);
    throw_if_error(status, "Failed to set time: ");
    m_listener = loop.get_io_listener(m_fd);
}

cppevent::timer::~timer() {
    int status = close(m_fd);
    throw_if_error(status, "Failed to close timer fd: ");
}

cppevent::awaitable_task<void> cppevent::timer::wait() {
    std::array<std::byte, 8> buf;
    auto status = co_await m_listener->on_read(buf.data(), 8);
    throw_if_error(status, "Timer wait failed: ");
}
