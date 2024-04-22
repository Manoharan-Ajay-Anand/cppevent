#include "pg_connection.hpp"

#include "types.hpp"

#include <cppevent_base/util.hpp>

#include <string>

constexpr long STARTUP_HEADER_SIZE = 8;
constexpr uint32_t POSTGRES_PROTOCOL_MAJOR_VERSION = 3;
constexpr uint32_t POSTGRES_PROTOCOL_MINOR_VERSION = 0;
constexpr uint32_t POSTGRES_PROTOCOL = (POSTGRES_PROTOCOL_MAJOR_VERSION << 16) + 
                                        POSTGRES_PROTOCOL_MINOR_VERSION;

constexpr char STARTUP_PARAM_USER[] = "user";
constexpr char STARTUP_PARAM_DATABASE[] = "database";

cppevent::pg_connection::~pg_connection() {
    if (m_sock) {
        --(*m_conn_count);
    }
}

cppevent::pg_connection::pg_connection(pg_connection&& other) {
    m_sock = std::move(other.m_sock);
    m_conn_count = other.m_conn_count;
}

cppevent::pg_connection& cppevent::pg_connection::operator=(pg_connection&& other) {
    m_sock = std::move(other.m_sock);
    m_conn_count = other.m_conn_count;
    return *this;
}

cppevent::awaitable_task<void> cppevent::pg_connection::init(std::unique_ptr<socket>&& sock,
                                                             long* conn_count,
                                                             const pg_config& config) {
    m_sock = std::move(sock);
    m_conn_count = conn_count;
    ++(*m_conn_count);

    std::string message;
    message.append(STARTUP_PARAM_USER, sizeof(STARTUP_PARAM_USER));
    message.append(config.m_user);
    message.push_back('\0');
    if (!config.m_database.empty()) {
        message.append(STARTUP_PARAM_DATABASE, sizeof(STARTUP_PARAM_DATABASE));
        message.append(config.m_database);
        message.push_back('\0');
    }
    message.push_back('\0');

    uint8_t header[STARTUP_HEADER_SIZE];
    write_u32_be(header, STARTUP_HEADER_SIZE + message.size());
    write_u32_be(header, POSTGRES_PROTOCOL);

    co_await m_sock->write(header, STARTUP_HEADER_SIZE);
    co_await m_sock->write(message.data(), message.size());
    co_await m_sock->flush();
}
