#ifndef CPPEVENT_BASE_EVENT_CALLBACK_HPP
#define CPPEVENT_BASE_EVENT_CALLBACK_HPP

#include "types.hpp"
#include "status_awaiter.hpp"

#include <optional>
#include <coroutine>
#include <memory>

namespace cppevent {

class event_bus;

class event_callback {
private:
    e_id m_id;
    event_bus& m_bus;
    std::optional<e_handler> m_handler_opt;
    awaiter_store m_store;
public:
    event_callback(event_bus& bus);
    ~event_callback();

    event_callback(const event_callback&) = delete;
    event_callback& operator=(const event_callback&) = delete;

    event_callback(event_callback&&) = delete;
    event_callback& operator=(event_callback&&) = delete;

    e_id get_id() const;

    void set_handler(const e_handler& handler);
    void notify(e_status status);

    status_awaiter await_status();
};

}

#endif
