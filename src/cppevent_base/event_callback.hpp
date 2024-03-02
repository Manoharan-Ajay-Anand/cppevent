#ifndef CPPEVENT_BASE_EVENT_CALLBACK_HPP
#define CPPEVENT_BASE_EVENT_CALLBACK_HPP

#include "types.hpp"

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
};

struct status_store {
    std::optional<e_status> m_status_opt;
    std::coroutine_handle<> m_handle;

    status_store(event_callback& callback);

    status_store(const status_store&) = delete;
    status_store& operator=(const status_store&) = delete;

    status_store(status_store&&) = delete;
    status_store& operator=(status_store&&) = delete;
};

struct status_awaiter {
    std::unique_ptr<status_store> m_store;

    status_awaiter(event_callback& callback);

    bool await_ready();

    void await_suspend(std::coroutine_handle<> handle);

    e_status await_resume();
};

}

#endif
