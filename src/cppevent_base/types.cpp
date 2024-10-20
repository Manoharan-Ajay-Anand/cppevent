#include "types.hpp"

constexpr uint64_t MASK = (1ull << 32) - 1;

cppevent::e_id::e_id(uint32_t index, uint32_t counter): m_index(index), m_counter(counter) {
}

cppevent::e_id::e_id(uint64_t val) {
    m_index = (val >> 32) & MASK;
    m_counter = val & MASK;
}

cppevent::e_id::operator uint64_t() const {
    return (static_cast<uint64_t>(m_index) << 32) + m_counter;
}
