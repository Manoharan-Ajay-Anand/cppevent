#ifndef CPPEVENT_BASE_IO_SERVICE_HPP
#define CPPEVENT_BASE_IO_SERVICE_HPP

#include "types.hpp"
#include "spin_lock.hpp"

#include <liburing.h>

#include <memory>
#include <queue>

namespace cppevent {

class event_bus;

class io_listener;

class io_service {
private:
    ::io_uring m_ring;
    int m_evfd;

    std::queue<e_event> m_events;
    spin_lock m_spin;
public:
    io_service();
    ~io_service();

    std::unique_ptr<io_listener> get_listener(int fd, event_bus& bus);

    void interrupt();
    void add_event(e_event ev);

    std::queue<e_event> poll_events();
};

}

#endif
