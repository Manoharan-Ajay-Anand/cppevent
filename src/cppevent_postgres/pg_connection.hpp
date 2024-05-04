#ifndef CPPEVENT_POSTGRES_PG_CONNECTION_HPP
#define CPPEVENT_POSTGRES_PG_CONNECTION_HPP

#include <cppevent_base/task.hpp>
#include <cppevent_net/socket.hpp>

#include <memory>

namespace cppevent {

enum class auth_type {
    OK = 0,
    CLEAR_TEXT_PASSWORD = 3,
    SASL = 10,
    SASL_CONTINUE,
    SASL_FINAL
};

enum class response_type: char {
    ERROR_RESPONSE = 'E',
    NEGOTIATE_PROTOCOL_VERSION = 'v',
    AUTHENTICATION = 'R',
    BACKEND_KEY_DATA = 'K',
    PARAMETER_STATUS = 'S',
    READY_FOR_QUERY = 'Z'
};

struct response_info {
    response_type m_type;
    long m_size;
};

struct pg_config;

class crypto;

class scram;

class pg_connection {
private:
    std::unique_ptr<socket> m_sock;
    long* m_conn_count;

    awaitable_task<void> handle_auth(response_info info,
                                     const pg_config& config, scram& scr);
public:
    pg_connection(std::unique_ptr<socket>&& sock, long* conn_count);
    ~pg_connection();

    pg_connection(pg_connection&& other);
    pg_connection& operator=(pg_connection&& other);

    awaitable_task<response_info> get_response_info();

    awaitable_task<void> init(const pg_config& config, crypto& crypt);

    awaitable_task<void> query(const std::string& q);
};

}

#endif
