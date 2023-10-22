#ifndef CPPEVENT_FCGI_RECORD_HPP
#define CPPEVENT_FCGI_RECORD_HPP

#include <cstdint>

namespace cppevent {

constexpr int FCGI_HEADER_LEN = 8;
constexpr int FCGI_BEGIN_REQ_LEN = 8;

constexpr int FCGI_VERSION_1 = 1;

constexpr int FCGI_BEGIN_REQUEST = 1;
constexpr int FCGI_ABORT_REQUEST = 2;
constexpr int FCGI_END_REQUEST = 3;
constexpr int FCGI_PARAMS = 4;
constexpr int FCGI_STDIN = 5;
constexpr int FCGI_STDOUT = 6;
constexpr int FCGI_STDERR = 7;
constexpr int FCGI_DATA = 8;
constexpr int FCGI_GET_VALUES = 9;
constexpr int FCGI_GET_VALUES_RESULT = 10;
constexpr int FCGI_UNKNOWN_TYPE = 11;
constexpr int FCGI_MAXTYPE = FCGI_UNKNOWN_TYPE;

constexpr int FCGI_NULL_REQUEST_ID = 0;

constexpr int FCGI_KEEP_CONN = 1;

constexpr int FCGI_MAX_PADDING = 255;

struct record {
    uint8_t m_version;
    uint8_t m_type;
    uint16_t m_req_id;
    uint16_t m_content_len;
    uint8_t m_padding_len;

    static record parse(const uint8_t* data);
    void serialize(uint8_t* data);
};

}

#endif
