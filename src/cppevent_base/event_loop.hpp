#ifndef CPPEVENT_BASE_EVENT_LOOP_HPP
#define CPPEVENT_BASE_EVENT_LOOP_HPP

#include "task.hpp"
#include "event_bus.hpp"
#include "io_listener.hpp"
#include "io_service.hpp"

#include <queue>

namespace cppevent {

class event_loop {
private:
    bool m_event_fd_triggered;
    bool m_running;
    int m_event_fd;

    std::queue<e_event> m_events;
    std::queue<std::function<void()>> m_ops;
    
    event_bus m_event_bus;
    io_service m_io_service;

    void trigger_event_fd();

    void run_ops();
    void notify_events();
    
    task<> run_internal_loop();

public:
    event_loop();
    ~event_loop();

    std::unique_ptr<io_listener> get_io_listener(int fd);
    event_callback get_event_callback();
    
    void add_event(e_event ev);
    void add_op(const std::function<void()>& op);
    
    void run();
    void stop();
};

}

#endif
