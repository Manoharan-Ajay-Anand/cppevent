#ifndef CPPEVENT_BASE_ASYNC_SIGNAL_HPP
#define CPPEVENT_BASE_ASYNC_SIGNAL_HPP

#include "event_loop.hpp"

#include <coroutine>
#include <optional>

namespace cppevent {

class signal_trigger {
private:
    e_id m_id;
    event_loop* m_loop = nullptr;

public:
    signal_trigger() = default;
    signal_trigger(e_id id, event_loop* loop);

    void notify() const;
};

class async_signal {
private:
    event_loop& m_loop;
    event_callback m_callback;

public:
    async_signal(event_loop& loop);

    async_signal(const async_signal&) = delete;
    async_signal& operator=(const async_signal&) = delete;

    async_signal(async_signal&&) = delete;
    async_signal& operator=(async_signal&&) = delete;

    signal_trigger get_trigger();
    status_awaiter await_signal();
};

}

#endif
