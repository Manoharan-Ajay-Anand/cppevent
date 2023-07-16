#include <iostream>
#include <array>

#include <cppevent_base/event_listener.hpp>
#include <cppevent_base/event_loop.hpp>
#include <cppevent_base/task.hpp>
#include <cppevent_base/timer.hpp>

using namespace std::chrono_literals;

cppevent::task<void> signal_coroutine(cppevent::event_listener& listener, bool& control) {
    while (control) {
        co_await cppevent::read_awaiter { listener };
        std::cout << "Received an in-app signal" << std::endl;
    }
}

cppevent::task<void> timed_coroutine(cppevent::event_loop& e_loop, bool& control) {
    auto& signal_listener = *(e_loop.get_signal_listener());
    auto t = signal_coroutine(signal_listener, control);
    cppevent::timer timer(3s, e_loop);
    std::cout << "Starting timed coroutine which triggers every 3 secs 5 times" << std::endl;
    for (int i = 1; i <= 5; ++i) {
        co_await timer.wait();
        std::cout << "Timer triggered: " << i << std::endl;
        e_loop.send_signal(&signal_listener, true, false);
    }
    control = false;
}

int main() {
    cppevent::event_loop e_loop;
    bool control = true;
    timed_coroutine(e_loop, control);
    e_loop.run(control);
    return 0;
}
