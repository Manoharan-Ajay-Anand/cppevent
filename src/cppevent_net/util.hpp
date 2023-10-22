#ifndef CPPEVENT_NET_UTIL_HPP
#define CPPEVENT_NET_UTIL_HPP

#include <netdb.h>

namespace cppevent {

void get_addrinfo(const char* name, const char* service, addrinfo** res, bool to_bind);

}

#endif
