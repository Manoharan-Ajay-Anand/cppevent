#ifndef CPPEVENT_BASE_TYPES_HPP
#define CPPEVENT_BASE_TYPES_HPP

#include <cstdint>
#include <functional>

namespace cppevent {

struct e_id {
    uint32_t m_index = 0;
    uint32_t m_counter = 0;

    e_id() = default;
    
    e_id(uint32_t index, uint32_t counter);
    explicit e_id(uint64_t val);

    explicit operator uint64_t() const;

    bool operator==(const e_id&) const = default;
};

using e_status = int64_t;

struct e_event {
    e_id m_id;
    e_status m_status;
};

using e_handler = std::function<void(e_status)>;

}

#endif
