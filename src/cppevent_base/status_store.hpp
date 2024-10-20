#ifndef CPPEVENT_BASE_STATUS_STORE_HPP
#define CPPEVENT_BASE_STATUS_STORE_HPP

#include "types.hpp"

#include <coroutine>
#include <queue>
#include <optional>

namespace cppevent {

class status_store {
private:
    e_id m_id;
    std::queue<status_store*>& m_released;

    std::coroutine_handle<> m_handle = std::noop_coroutine();
    std::optional<e_status> m_status_opt;

public:
    status_store(e_id id, std::queue<status_store*>& released);

    status_store(const status_store&) = delete;
    status_store& operator=(const status_store&) = delete;

    status_store(status_store&&) = delete;
    status_store& operator=(status_store&&) = delete;

    e_id get_id() const;

    void notify(e_status status);

    void set_handle(std::coroutine_handle<> h);

    bool has_status() const;

    e_status consume_status();

    void reset();

    void release();
};

}

#endif
