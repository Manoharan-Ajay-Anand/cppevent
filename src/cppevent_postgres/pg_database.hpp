#ifndef CPPEVENT_POSTGRES_PG_DATABASE_HPP
#define CPPEVENT_POSTGRES_PG_DATABASE_HPP

#include "types.hpp"
#include "pg_connection.hpp"

#include <cppevent_base/task.hpp>
#include <cppevent_base/types.hpp>
#include <cppevent_net/client_socket.hpp>
#include <cppevent_crypto/crypto.hpp>

#include <memory>
#include <queue>

namespace cppevent {

class pg_database {
private:
    event_loop& m_loop;
    crypto m_crypt;
    pg_config m_config;
    client_socket m_client_sock;

    long m_conn_count = 0;
    std::queue<pg_connection> m_idle;
    std::queue<e_id> m_waiting;
public:
    pg_database(const char* name, const char* service, const pg_config& config, event_loop& loop);
    pg_database(const char* unix_path, const pg_config& config, event_loop& loop);

    task<pg_connection> get_connection();
    void release(pg_connection& conn);
};

}

#endif
