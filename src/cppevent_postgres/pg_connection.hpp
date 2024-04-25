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
    AUTHENTICATION = 'R'
};

struct response_info {
    response_type m_type;
    long m_size;
};

struct sasl_context {
    std::string m_client_nonce;
    std::string m_client_first_msg_bare;
    std::string m_server_first_msg;
};

struct pg_config;

class pg_connection {
private:
    std::unique_ptr<socket> m_sock;
    long* m_conn_count = nullptr;

    awaitable_task<response_info> get_response_info();

    awaitable_task<bool> handle_auth(response_info info,
                                     const pg_config& config, sasl_context& context);
public:
    pg_connection() = default;
    ~pg_connection();

    pg_connection(pg_connection&& other);
    pg_connection& operator=(pg_connection&& other);

    awaitable_task<void> init(std::unique_ptr<socket>&& sock, long* conn_count,
                              const pg_config& config);
};

}

#endif
