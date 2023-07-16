#include <iostream>
#include <array>

#include <cppevent_base/event_listener.hpp>
#include <cppevent_base/event_loop.hpp>
#include <cppevent_base/task.hpp>
#include <cppevent_base/timer.hpp>

using namespace std::chrono_literals;

cppevent::task<void> signal_coroutine(cppevent::event_listener& listener) {
    for (int i = 1; i <= 5; ++i) {
        co_await cppevent::read_awaiter { listener };
        std::cout << "Received an in-app signal: " << i << std::endl;
    }
}

cppevent::task<void> timed_coroutine(cppevent::event_loop& e_loop) {
    bool control = true;
    auto& signal_listener = *(e_loop.get_signal_listener());
    auto t = signal_coroutine(signal_listener);
    cppevent::timer timer(3s, e_loop);
    std::cout << "Starting timed coroutine which triggers every 3 secs 5 times" << std::endl;
    while (true) {
        co_await timer.wait();
        if (t.m_handle.done()) {
            break;
        }
        std::cout << "Timer triggered" << std::endl;
        e_loop.send_signal(&signal_listener, true, false);
    }
    e_loop.stop();
}

int main() {
    cppevent::event_loop e_loop;
    timed_coroutine(e_loop);
    e_loop.run();
    return 0;
}
