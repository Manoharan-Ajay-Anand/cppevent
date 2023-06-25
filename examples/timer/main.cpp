#include <iostream>
#include <array>

#include <cppevent_base/event_listener.hpp>
#include <cppevent_base/event_loop.hpp>
#include <cppevent_base/task.hpp>
#include <cppevent_base/util.hpp>

#include <sys/timerfd.h>
#include <unistd.h>

int create_timer_fd(time_t seconds) {
    int t_fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK);
    cppevent::throw_if_error(t_fd, "Failed to create timer fd");
    timespec t_spec;
    t_spec.tv_sec = seconds;
    t_spec.tv_nsec = 0;
    itimerspec i_spec = { t_spec, t_spec };
    int status = timerfd_settime(t_fd, 0, &i_spec, NULL);
    cppevent::throw_if_error(status, "Failed to set time");
    return t_fd;
}

cppevent::task timed_coroutine(cppevent::event_loop& e_loop) {
    int t_fd = create_timer_fd(2);
    auto& listener = *(e_loop.get_listener(t_fd));
    int count = 1;
    std::cout << "This should print every 2 seconds: " << count << std::endl;
    while (true) {
        std::array<std::byte, 8> buf;
        int status = read(t_fd, buf.data(), 8);
        if (status > 0) { 
            std::cout << "This should print every 2 seconds: " << ++count << std::endl;
            continue;
        }
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            co_await cppevent::read_awaiter { listener };
        } else {
            break;
        }
    }
    cppevent::throw_errno("Failed to read timer fd");
}

int main() {
    cppevent::event_loop e_loop;
    timed_coroutine(e_loop);
    e_loop.run();
    return 0;
}
