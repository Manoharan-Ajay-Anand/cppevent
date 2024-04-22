#include "pg_connection.hpp"

cppevent::pg_connection::pg_connection(std::unique_ptr<socket> sock,
                                       long& conn_count): m_sock(std::move(sock)),
                                                          m_conn_count(conn_count) {
}

cppevent::pg_connection::pg_connection(long& conn_count): m_conn_count(conn_count) {
}

cppevent::pg_connection::~pg_connection() {
    if (m_sock) {
        --m_conn_count;
    }
}

cppevent::pg_connection::pg_connection(pg_connection&& other): m_conn_count(other.m_conn_count) {
    m_sock = std::move(other.m_sock);
}

cppevent::awaitable_task<void> cppevent::pg_connection::init(std::unique_ptr<socket>&& sock) {
    m_sock = std::move(sock);
    ++m_conn_count;
}
