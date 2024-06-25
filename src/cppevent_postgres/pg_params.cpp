#include "pg_params.hpp"

#include <cppevent_base/util.hpp>

void cppevent::pg_params::increment() {
    write_u16_be(m_buffer.data(), ++m_count);
}

long cppevent::pg_params::size() const {
    return m_buffer.size();
}

const uint8_t* cppevent::pg_params::data() const {
    return m_buffer.data();
}
