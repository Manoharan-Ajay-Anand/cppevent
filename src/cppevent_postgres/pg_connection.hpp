#ifndef CPPEVENT_POSTGRES_PG_CONNECTION_HPP
#define CPPEVENT_POSTGRES_PG_CONNECTION_HPP

#include <cppevent_base/task.hpp>
#include <cppevent_net/socket.hpp>

#include <memory>

namespace cppevent {

class pg_connection {
private:
    std::unique_ptr<socket> m_sock;
    long& m_conn_count;
public:
    pg_connection(std::unique_ptr<socket> sock, long& conn_count);
    pg_connection(long& conn_count);

    ~pg_connection();

    pg_connection(pg_connection&& other);

    awaitable_task<void> init(std::unique_ptr<socket>&& sock);
};

}

#endif
