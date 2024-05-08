#ifndef CPPEVENT_BASE_SUSPENDED_CORO_HPP
#define CPPEVENT_BASE_SUSPENDED_CORO_HPP

#include <coroutine>

namespace cppevent {

class suspended_coro {
private:
    std::coroutine_handle<> m_handle = std::noop_coroutine();
    bool m_stored = false;

public:
    bool has_handle() const;

    void store_handle(const std::coroutine_handle<>& handle);
    std::coroutine_handle<> retrieve_handle();

    void reset();
};

}

#endif
