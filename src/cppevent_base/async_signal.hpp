#ifndef CPPEVENT_BASE_ASYNC_SIGNAL_HPP
#define CPPEVENT_BASE_ASYNC_SIGNAL_HPP

#include "event_loop.hpp"

#include <coroutine>
#include <optional>

namespace cppevent {

struct signal_trigger {
    const e_id m_id;
    event_loop* const m_loop;

    void activate() const;
};

struct signal_awaiter {
    bool& m_triggered;
    std::optional<std::coroutine_handle<>>& m_waiting_opt;

    bool await_ready();

    void await_suspend(std::coroutine_handle<> handle);

    void await_resume();
};

class async_signal {
private:
    bool m_triggered = false;
    std::optional<std::coroutine_handle<>> m_waiting_opt;

    event_callback m_callback;
    event_loop& m_loop;

    void init_handler();
public:
    async_signal(event_loop& loop);

    async_signal(const async_signal&) = delete;
    async_signal& operator=(const async_signal&) = delete;

    async_signal(async_signal&&) = delete;
    async_signal& operator=(async_signal&&) = delete;

    signal_trigger get_trigger();
    signal_awaiter await_signal();
};

}

#endif
