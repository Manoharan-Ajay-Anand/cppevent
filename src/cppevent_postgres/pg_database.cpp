#include "pg_database.hpp"

#include <cppevent_base/event_loop.hpp>

cppevent::pg_database::pg_database(const char* name,
                                   const char* service,
                                   const pg_config& config,
                                   event_loop& loop): m_loop(loop),
                                                      m_config(config), 
                                                      m_client_sock(name, service, loop) {
}

cppevent::pg_database::pg_database(const char* unix_path,
                                   const pg_config& config,
                                   event_loop& loop): m_loop(loop),
                                                      m_config(config), 
                                                      m_client_sock(unix_path, loop) {
}

cppevent::task<cppevent::pg_connection> cppevent::pg_database::get_connection() {
    if (m_idle.empty() && m_conn_count < m_config.m_max_connections) {
        pg_connection conn { co_await m_client_sock.connect(), &m_conn_count };
        co_await conn.init(m_config, m_crypt);
        co_return std::move(conn);
    }

    if (m_idle.empty()) {
        event_callback callback = m_loop.get_event_callback();
        m_waiting.push(callback.get_id());
        co_await callback.await_status();
    }
    pg_connection conn = std::move(m_idle.front());
    m_idle.pop();
    co_return std::move(conn);
}

void cppevent::pg_database::release(pg_connection& conn) {
    m_idle.push(std::move(conn));
    if (!m_waiting.empty()) {
        e_id id = m_waiting.front();
        m_waiting.pop();
        m_loop.add_event({ id, 0 });
    }
}
