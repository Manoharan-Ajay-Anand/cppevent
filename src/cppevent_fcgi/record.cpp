#include "record.hpp"

#include <cppevent_base/util.hpp>

cppevent::record cppevent::record::parse(const uint8_t* data) {
    uint16_t id = read_u16_be(data + 2);
    uint16_t content_len = read_u16_be(data + 4);
    return { *data, *(data + 1), id, content_len, *(data + 6) };
}

void cppevent::record::serialize(uint8_t* data) {
    *data = m_version;
    *(data + 1) = m_type;
    write_u16_be(data + 2, m_req_id);
    write_u16_be(data + 4, m_content_len);
    *(data + 6) = m_padding_len;
}
