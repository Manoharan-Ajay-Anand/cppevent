#ifndef CPPEVENT_FCGI_OUTPUT_CONTROL_HPP
#define CPPEVENT_FCGI_OUTPUT_CONTROL_HPP

#include <queue>
#include <coroutine>

namespace cppevent {

class event_loop;

struct output_control_awaiter {
    bool& m_available;
    std::queue<std::coroutine_handle<>>& m_waiting;

    bool await_ready();

    void await_suspend(std::coroutine_handle<> handle);

    void await_resume();
};

class output_control {
private:
    bool m_available = true;
    std::queue<std::coroutine_handle<>> m_waiting;
    event_loop& m_loop;
public:
    output_control(event_loop& loop);

    output_control_awaiter lock();

    bool has_pending() const;

    void release();
};

}

#endif
