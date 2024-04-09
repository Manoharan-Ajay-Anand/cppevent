#ifndef CPPEVENT_BASE_EVENT_BUS_HPP
#define CPPEVENT_BASE_EVENT_BUS_HPP

#include "types.hpp"

#include <unordered_map>

namespace cppevent {

class event_callback;

class event_bus {
private:
    e_id m_id_counter = 0;
    std::unordered_map<e_id, event_callback*> m_callbacks;

public:
    e_id register_event_callback(event_callback* callback);
    void deregister_event_callback(e_id id);
    
    void notify(e_id id, e_status status);
};

}

#endif
