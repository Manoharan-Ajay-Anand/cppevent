#ifndef CPPEVENT_NET_TYPES_HPP
#define CPPEVENT_NET_TYPES_HPP

namespace cppevent {

enum class OP_STATUS {
    SUCCESS,
    BLOCK,
    CLOSE,
    ERROR
};

constexpr long SOCKET_BUFFER_SIZE = 8192;

}

#endif
