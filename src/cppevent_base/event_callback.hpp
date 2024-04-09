#ifndef CPPEVENT_BASE_EVENT_CALLBACK_HPP
#define CPPEVENT_BASE_EVENT_CALLBACK_HPP

#include "types.hpp"
#include "status_awaiter.hpp"

#include <optional>
#include <coroutine>

namespace cppevent {

class event_bus;

class event_callback {
private:
    e_id m_id;
    event_bus& m_bus;

    std::optional<e_status> m_status_opt;
    std::optional<std::coroutine_handle<>> m_handle_opt;
public:
    event_callback(event_bus& bus);
    ~event_callback();

    event_callback(const event_callback&) = delete;
    event_callback& operator=(const event_callback&) = delete;

    event_callback(event_callback&&) = delete;
    event_callback& operator=(event_callback&&) = delete;

    e_id get_id() const;

    void set_handle(std::coroutine_handle<> handle);
    void notify(e_status status);

    bool has_status() const;
    e_status get_status();
    void reset();

    status_awaiter await_status();
};

}

#endif
