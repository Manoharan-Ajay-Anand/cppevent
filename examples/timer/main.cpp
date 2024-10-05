#include <iostream>
#include <array>

#include <cppevent_base/async_queue.hpp>
#include <cppevent_base/event_loop.hpp>
#include <cppevent_base/task.hpp>
#include <cppevent_base/timer.hpp>

using namespace std::chrono_literals;

cppevent::task<> queue_coroutine(cppevent::async_queue<int>& aq) {
    while(true) {
        co_await aq.await_items();
        std::cout << "Received item in async queue: " << aq.front() << std::endl;
        aq.pop();
    }
}

cppevent::task<> timed_coroutine(cppevent::event_loop& e_loop) {
    cppevent::async_queue<int> aq(e_loop);
    cppevent::task<> t = queue_coroutine(aq);
    cppevent::timer timer(2s, e_loop);
    std::cout << "Starting timed coroutine which triggers every 2 secs 5 times" << std::endl;
    for (int i = 1; i <= 5; ++i) {
        co_await timer.wait();
        std::cout << "Timer triggered: " << i << std::endl;
        aq.push(i);
        aq.push(i * 2);
    }
    co_await timer.wait();
    e_loop.stop();
}

int main() {
    cppevent::event_loop e_loop;
    cppevent::task<> t = timed_coroutine(e_loop);
    e_loop.run();
    return 0;
}
