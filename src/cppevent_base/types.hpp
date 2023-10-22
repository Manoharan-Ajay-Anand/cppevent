#ifndef CPPEVENT_BASE_TYPES_HPP
#define CPPEVENT_BASE_TYPES_HPP

#include <cstdint>
#include <functional>

namespace cppevent {

using e_id = uint64_t;

using e_status = int64_t;

struct e_event {
    e_id m_id;
    e_status m_status;
};

using e_handler = std::function<void(e_status)>;

}

#endif
