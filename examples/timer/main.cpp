#include <iostream>
#include <array>

#include <cppevent_base/event_listener.hpp>
#include <cppevent_base/event_loop.hpp>
#include <cppevent_base/task.hpp>
#include <cppevent_base/timer.hpp>

using namespace std::chrono_literals;

cppevent::awaitable_task<void> signal_coroutine(cppevent::event_listener* listener) {
    while(true) {
        co_await cppevent::read_awaiter { *listener };
        std::cout << "Received an in-app signal" << std::endl;
    }
}

cppevent::task timed_coroutine(cppevent::event_loop& e_loop) {
    cppevent::event_listener* signal_listener = e_loop.get_signal_listener();
    auto s_id = signal_listener->get_id();
    cppevent::awaitable_task<void> t = signal_coroutine(signal_listener);
    cppevent::timer timer(2s, e_loop);
    std::cout << "Starting timed coroutine which triggers every 2 secs 5 times" << std::endl;
    for (int i = 1; i <= 5; ++i) {
        co_await timer.wait();
        std::cout << "Timer triggered: " << i << std::endl;
        e_loop.send_signal(s_id, true, false);
    }
    co_await timer.wait();
    signal_listener->detach();
    e_loop.stop();
}

int main() {
    cppevent::event_loop e_loop;
    timed_coroutine(e_loop);
    e_loop.run();
    return 0;
}
