#ifndef CPPEVENT_FCGI_TYPES_HPP
#define CPPEVENT_FCGI_TYPES_HPP

#include <coroutine>
#include <optional>
#include <unordered_map>
#include <memory>

namespace cppevent {

class request;

enum class REQUEST_METHOD {
    GET,
    POST
};

using request_map = std::unordered_map<int, std::unique_ptr<request>>;

}

#endif
