#include "types.hpp"

#include <cctype>
#include <stdexcept>

size_t cppevent::case_insensitive_hash::operator()(const std::string_view& key) const{
    size_t seed = 0;
    std::hash<char> hasher;
    for (char c : key) {
        seed ^= hasher(std::tolower(c)) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    }
    return seed;
}

bool cppevent::case_insensitive_equality::operator()(const std::string_view& a,
                                                     const std::string_view& b) const{
    if (a.size() != b.size()) {
        return false;
    }
    for (long i = 0; i < a.size(); ++i) {
        if (std::tolower(a[i]) != std::tolower(b[i])) {
            return false;
        }
    }
    return true;
}

std::string_view cppevent::get_status_reason_phrase(HTTP_STATUS status) {
    switch (status) {
        case HTTP_STATUS::CONTINUE:
            return "100 Continue";
        case HTTP_STATUS::SWITCHING_PROTOCOLS:
            return "101 Switching Protocols";
        case HTTP_STATUS::OK:
            return "200 OK";
        case HTTP_STATUS::CREATED:
            return "201 Created";
        case HTTP_STATUS::ACCEPTED:
            return "202 Accepted";
        case HTTP_STATUS::NON_AUTHORITATIVE_INFORMATION:
            return "203 Non-Authoritative Information";
        case HTTP_STATUS::NO_CONTENT:
            return "204 No Content";
        case HTTP_STATUS::RESET_CONTENT:
            return "205 Reset Content";
        case HTTP_STATUS::PARTIAL_CONTENT:
            return "206 Partial Content";
        case HTTP_STATUS::MULTIPLE_CHOICES:
            return "300 Multiple Choices";
        case HTTP_STATUS::MOVED_PERMANENTLY:
            return "301 Moved Permanently";
        case HTTP_STATUS::FOUND:
            return "302 Found";
        case HTTP_STATUS::SEE_OTHER:
            return "303 See Other";
        case HTTP_STATUS::NOT_MODIFIED:
            return "304 Not Modified";
        case HTTP_STATUS::USE_PROXY:
            return "305 Use Proxy";
        case HTTP_STATUS::TEMPORARY_REDIRECT:
            return "307 Temporary Redirect";
        case HTTP_STATUS::BAD_REQUEST:
            return "400 Bad Request";
        case HTTP_STATUS::UNAUTHORIZED:
            return "401 Unauthorized";
        case HTTP_STATUS::PAYMENT_REQUIRED:
            return "402 Payment Required";
        case HTTP_STATUS::FORBIDDEN:
            return "403 Forbidden";
        case HTTP_STATUS::NOT_FOUND:
            return "404 Not Found";
        case HTTP_STATUS::METHOD_NOT_ALLOWED:
            return "405 Method Not Allowed";
        case HTTP_STATUS::NOT_ACCEPTABLE:
            return "406 Not Acceptable";
        case HTTP_STATUS::PROXY_AUTHENTICATION_REQUIRED:
            return "407 Proxy Authentication Required";
        case HTTP_STATUS::REQUEST_TIMEOUT:
            return "408 Request Time-out";
        case HTTP_STATUS::CONFLICT:
            return "409 Conflict";
        case HTTP_STATUS::GONE:
            return "410 Gone";
        case HTTP_STATUS::LENGTH_REQUIRED:
            return "411 Length Required";
        case HTTP_STATUS::PRECONDITION_FAILED:
            return "412 Precondition Failed";
        case HTTP_STATUS::REQUEST_ENTITY_TOO_LARGE:
            return "413 Request Entity Too Large";
        case HTTP_STATUS::REQUEST_URI_TOO_LARGE:
            return "414 Request-URI Too Large";
        case HTTP_STATUS::UNSUPPORTED_MEDIA_TYPE:
            return "415 Unsupported Media Type";
        case HTTP_STATUS::REQUESTED_RANGE_NOT_SATISFIABLE:
            return "416 Requested range not satisfiable";
        case HTTP_STATUS::EXPECTATION_FAILED:
            return "417 Expectation Failed";
        case HTTP_STATUS::INTERNAL_SERVER_ERROR:
            return "500 Internal Server Error";
        case HTTP_STATUS::NOT_IMPLEMENTED:
            return "501 Not Implemented";
        case HTTP_STATUS::BAD_GATEWAY:
            return "502 Bad Gateway";
        case HTTP_STATUS::SERVICE_UNAVAILABLE:
            return "503 Service Unavailable";
        case HTTP_STATUS::GATEWAY_TIMEOUT:
            return "504 Gateway Time-out";
        case HTTP_STATUS::HTTP_VERSION_NOT_SUPPORTED:
            return "505 HTTP Version not supported";
        default:
            throw std::runtime_error("get_status_reason_phrase: unknown status code");
    }
}
