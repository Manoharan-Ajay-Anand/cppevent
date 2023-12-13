#ifndef CPPEVENT_BASE_IO_SERVICE_HPP
#define CPPEVENT_BASE_IO_SERVICE_HPP

#include <liburing.h>

#include <memory>

#include "types.hpp"

namespace cppevent {

class event_bus;

class io_listener;

class io_service {
private:
    io_uring m_ring;
public:
    io_service();
    ~io_service();

    std::unique_ptr<io_listener> get_listener(int fd, event_bus& bus);

    e_event poll();
};

}

#endif
