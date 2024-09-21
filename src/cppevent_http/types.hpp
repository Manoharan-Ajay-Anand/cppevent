#ifndef CPPEVENT_HTTP_TYPES_HPP
#define CPPEVENT_HTTP_TYPES_HPP

#include <string_view>
#include <string>
#include <unordered_map>

namespace cppevent {

constexpr int HEX_BASE = 16;

struct case_insensitive_hash {
    using is_transparent = void;

    size_t operator()(const std::string_view& key) const;
};

struct case_insensitive_equality {
    using is_transparent = void;
    
    bool operator()(const std::string_view& a, const std::string_view& b) const;
};

using header_map = std::unordered_map<std::string, std::string,
                                      case_insensitive_hash, case_insensitive_equality>;

enum class HTTP_METHOD {
    GET,
    POST,
    HEAD,
    OPTIONS
};

enum class HTTP_VERSION {
    HTTP_1_0,
    HTTP_1_1
};

enum class HTTP_STATUS {
    CONTINUE = 100,
    SWITCHING_PROTOCOLS,
    OK = 200,
    CREATED,
    ACCEPTED,
    NON_AUTHORITATIVE_INFORMATION,
    NO_CONTENT,
    RESET_CONTENT,
    PARTIAL_CONTENT,
    MULTIPLE_CHOICES = 300,
    MOVED_PERMANENTLY,
    FOUND,
    SEE_OTHER,
    NOT_MODIFIED,
    USE_PROXY,
    TEMPORARY_REDIRECT = 307,
    BAD_REQUEST = 400,
    UNAUTHORIZED,
    PAYMENT_REQUIRED,
    FORBIDDEN,
    NOT_FOUND,
    METHOD_NOT_ALLOWED,
    NOT_ACCEPTABLE,
    PROXY_AUTHENTICATION_REQUIRED,
    REQUEST_TIMEOUT,
    CONFLICT,
    GONE,
    LENGTH_REQUIRED,
    PRECONDITION_FAILED,
    REQUEST_ENTITY_TOO_LARGE,
    REQUEST_URI_TOO_LARGE,
    UNSUPPORTED_MEDIA_TYPE,
    REQUESTED_RANGE_NOT_SATISFIABLE,
    EXPECTATION_FAILED,
    INTERNAL_SERVER_ERROR = 500,
    NOT_IMPLEMENTED,
    BAD_GATEWAY,
    SERVICE_UNAVAILABLE,
    GATEWAY_TIMEOUT,
    HTTP_VERSION_NOT_SUPPORTED,
    INSUFFICIENT_STORAGE = 507
};

std::string_view get_status_reason_phrase(HTTP_STATUS status);

}

#endif
