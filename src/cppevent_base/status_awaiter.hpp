#ifndef CPPEVENT_BASE_STATUS_AWAITER_HPP
#define CPPEVENT_BASE_STATUS_AWAITER_HPP

#include "types.hpp"

#include <optional>
#include <coroutine>

namespace cppevent {

class event_callback;

class status_awaiter {
private:
    event_callback* m_callback;

public:
    status_awaiter(event_callback* callback);
    ~status_awaiter();

    status_awaiter(const status_awaiter&) = delete;
    status_awaiter& operator=(const status_awaiter&) = delete;

    status_awaiter(status_awaiter&&);
    status_awaiter& operator=(status_awaiter&&);

    bool await_ready();

    void await_suspend(std::coroutine_handle<> handle);

    e_status await_resume();
};

}

#endif
