#ifndef CPPEVENT_BASE_EVENT_CALLBACK_HPP
#define CPPEVENT_BASE_EVENT_CALLBACK_HPP

#include "types.hpp"
#include "status_awaiter.hpp"

#include <optional>
#include <coroutine>

namespace cppevent {

class status_store;

class event_callback {
private:
    status_store* m_store;

public:
    event_callback(status_store* store);
    ~event_callback();

    event_callback(const event_callback&) = delete;
    event_callback& operator=(const event_callback&) = delete;

    event_callback(event_callback&&) = delete;
    event_callback& operator=(event_callback&&) = delete;

    e_id get_id() const;

    status_awaiter await_status();
};

}

#endif
