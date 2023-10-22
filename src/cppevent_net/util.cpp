#include "util.hpp"

#include <stdexcept>

void cppevent::get_addrinfo(const char* name, const char* service,
                            addrinfo** res, bool to_bind) {
    addrinfo hints {};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_ADDRCONFIG;
    if (to_bind) {
        hints.ai_flags |= AI_PASSIVE;
    }

    int status = ::getaddrinfo(name, service, &hints, res);
    if (status != 0) {
        throw std::runtime_error(
                std::string("getaddrinfo failed: ").append(::gai_strerror(status)));
    }
}
