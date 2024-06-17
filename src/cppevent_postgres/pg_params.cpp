#include "pg_params.hpp"

long cppevent::pg_params::count() const {
    return m_count;
}

long cppevent::pg_params::size() const {
    return m_buffer.size();
}

const uint8_t* cppevent::pg_params::data() const {
    return m_buffer.data();
}
