#ifndef CPPEVENT_BASE_EVENT_LOOP_HPP
#define CPPEVENT_BASE_EVENT_LOOP_HPP

#include "task.hpp"
#include "event_bus.hpp"
#include "io_listener.hpp"
#include "io_service.hpp"

namespace cppevent {

class event_loop {
private:
    bool m_running = true;
    
    event_bus m_event_bus;
    io_service m_io_service;

public:
    std::unique_ptr<io_listener> get_io_listener(int fd);
    event_callback get_event_callback();
    
    void add_event(e_event ev);
    
    void run();
    void stop();
};

}

#endif
