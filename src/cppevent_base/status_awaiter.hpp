#ifndef CPPEVENT_BASE_STATUS_AWAITER_HPP
#define CPPEVENT_BASE_STATUS_AWAITER_HPP

#include "types.hpp"

#include <optional>
#include <coroutine>

namespace cppevent {

class awaiter_store {
private:
    bool m_is_set;
    std::optional<e_status> m_status_opt;
    std::coroutine_handle<> m_handle;

public:
    awaiter_store();

    void prepare();

    void set_handle(std::coroutine_handle<> handle);

    void resume(e_status status);

    bool has_status();
    e_status get_status();

    void reset();
};

class status_awaiter {
private:
    awaiter_store* m_store;

public:
    status_awaiter(awaiter_store* store);
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
