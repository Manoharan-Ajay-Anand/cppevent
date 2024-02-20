#ifndef CPPEVENT_BASE_EVENT_CALLBACK_HPP
#define CPPEVENT_BASE_EVENT_CALLBACK_HPP

#include "types.hpp"

#include <optional>
#include <coroutine>

namespace cppevent {

class event_bus;

class event_callback {
private:
    e_id m_id;
    event_bus& m_bus;
    std::optional<e_status> m_status_opt;
    std::optional<e_handler> m_handler_opt;
public:
    event_callback(event_bus& bus);
    ~event_callback();

    event_callback(const event_callback&) = delete;
    event_callback& operator=(const event_callback&) = delete;

    event_callback(event_callback&&) = delete;
    event_callback& operator=(event_callback&&) = delete;

    e_id get_id() const;

    std::optional<e_status> get_status();

    void set_handler(const e_handler& handler);
    void notify(e_status status);
};

struct status_awaiter {
    event_callback& m_callback;
    e_status m_status = 0;

    bool await_ready();

    void await_suspend(std::coroutine_handle<> handle);

    e_status await_resume();
};

}

#endif
