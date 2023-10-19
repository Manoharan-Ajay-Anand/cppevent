#ifndef CPPEVENT_FCGI_TYPES_HPP
#define CPPEVENT_FCGI_TYPES_HPP

#include "record.hpp"

#include <cstddef>

#include <cppevent_base/async_signal.hpp>
#include <cppevent_base/async_queue.hpp>

namespace cppevent {

struct output_cmd {
    bool m_close_conn;
    record m_record;
    const void* m_content;
    long m_content_len;
    signal_trigger m_trigger;
};

using output_queue = async_queue<output_cmd>;

enum class REQUEST_METHOD {
    GET,
    POST
};

}

#endif
