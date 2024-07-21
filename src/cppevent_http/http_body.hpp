#ifndef CPPEVENT_HTTP_HTTP_BODY_HPP
#define CPPEVENT_HTTP_HTTP_BODY_HPP

#include <cppevent_base/task.hpp>

#include <string>

namespace cppevent {

class socket;

class http_body {
private:
    long m_incoming;
    bool m_ended;
    socket& m_sock;

    awaitable_task<bool> has_incoming();
public:
    http_body(long incoming, bool ended, socket& sock);

    awaitable_task<long> read(void* dest, long size);
    awaitable_task<long> read(std::string& dest, long size);
    awaitable_task<long> skip(long size);
};

}

#endif
