#ifndef CPPEVENT_BASE_ASYNC_SIGNAL_HPP
#define CPPEVENT_BASE_ASYNC_SIGNAL_HPP

#include "event_loop.hpp"

#include <coroutine>

namespace cppevent {

struct signal_trigger {
    const e_id m_id;
    event_loop* const m_loop;

    void activate() {
        m_loop->add_event({ m_id, 0 });
    }
};

struct signal_awaiter {
private:
    bool& m_triggered;
    std::coroutine_handle<>& m_handle;
public:
    signal_awaiter(bool& triggered, std::coroutine_handle<>& handle);

    bool await_ready();
    
    void await_suspend(std::coroutine_handle<> handle);
    
    void await_resume();
};

class async_signal {
private:
    event_callback* m_callback;
    event_loop& m_loop;
    bool m_triggered;
    std::coroutine_handle<> m_handle;

    void set_callback();

public:
    async_signal(event_loop& loop);
    ~async_signal();

    async_signal(const async_signal&) = delete;
    async_signal& operator=(const async_signal&) = delete;

    async_signal(async_signal&&) = delete;
    async_signal& operator=(async_signal&&) = delete;

    signal_trigger get_trigger();
    signal_awaiter await_signal();
};

}

#endif
