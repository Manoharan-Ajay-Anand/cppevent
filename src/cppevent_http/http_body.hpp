#ifndef CPPEVENT_HTTP_HTTP_BODY_HPP
#define CPPEVENT_HTTP_HTTP_BODY_HPP

#include <cppevent_base/task.hpp>

namespace cppevent {

class socket;

class http_body {
private:
    long m_available;
    bool m_ended;
    socket& m_sock;

    awaitable_task<long> get_available();
public:
    http_body(long available, bool ended, socket& sock);
};

}

#endif
