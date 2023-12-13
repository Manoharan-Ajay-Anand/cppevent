#ifndef CPPEVENT_BASE_EVENT_BUS_HPP
#define CPPEVENT_BASE_EVENT_BUS_HPP

#include "types.hpp"

#include "event_callback.hpp"

#include <unordered_map>
#include <queue>

namespace cppevent {

class event_bus {
private:
    e_id m_id_counter = 0;
    std::unordered_map<e_id, event_callback> m_callbacks;
    std::queue<e_id> m_marked_deletion;
public:
    event_callback* get_event_callback();
    void remove_event_callback(e_id id);
    
    void notify(e_id id, e_status status);
};

}

#endif
