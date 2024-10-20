#ifndef CPPEVENT_BASE_EVENT_BUS_HPP
#define CPPEVENT_BASE_EVENT_BUS_HPP

#include "status_store.hpp"
#include "event_callback.hpp"

#include <queue>
#include <memory>

namespace cppevent {

class event_bus {
private:
    std::queue<status_store*> m_released;
    std::vector<std::unique_ptr<status_store>> m_stores;

public:
    event_callback get_event_callback();
    
    void notify(e_id id, e_status status);
};

}

#endif
