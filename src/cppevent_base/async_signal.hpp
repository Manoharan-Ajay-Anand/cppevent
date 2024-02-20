#ifndef CPPEVENT_BASE_ASYNC_SIGNAL_HPP
#define CPPEVENT_BASE_ASYNC_SIGNAL_HPP

#include "event_loop.hpp"

#include <coroutine>

namespace cppevent {

struct signal_trigger {
    const e_id m_id;
    event_loop* const m_loop;

    void activate();
};

class async_signal {
private:
    event_callback m_callback;
    event_loop& m_loop;

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
