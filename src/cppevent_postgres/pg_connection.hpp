#ifndef CPPEVENT_POSTGRES_PG_CONNECTION_HPP
#define CPPEVENT_POSTGRES_PG_CONNECTION_HPP

#include "pg_result.hpp"

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
    READY_FOR_QUERY = 'Z',
    NOTICE_RESPONSE = 'N',
    EMPTY_QUERY_RESPONSE = 'I',
    COMMAND_COMPLETE = 'C',
    DATA_ROW = 'D',
    ROW_DESCRIPTION = 'T',
    PARSE_COMPLETE = '1',
    BIND_COMPLETE = '2',
    PORTAL_SUSPENDED = 's'
};

struct response_info {
    response_type m_type;
    long m_size;
};

constexpr int HEADER_SIZE = 5;

class response_header {
private:
    uint8_t m_buf[HEADER_SIZE];
public:
    void set_type(char c);

    void set_size(long size);

    const uint8_t* data();

    constexpr long size() const {
        return HEADER_SIZE;
    }
};

struct pg_config;

class crypto;

class scram;

class pg_params;

class pg_connection {
private:
    std::unique_ptr<socket> m_sock;
    long* m_conn_count;
    bool m_query_ready = false;

    awaitable_task<void> handle_auth(response_info info,
                                     const pg_config& config, scram& scr);

    awaitable_task<response_info> get_response_info(bool ignore_notice = true);

    awaitable_task<void> populate_result(response_info info, pg_result& result);

public:
    pg_connection(std::unique_ptr<socket>&& sock, long* conn_count);
    ~pg_connection();

    pg_connection(pg_connection&& other);
    pg_connection& operator=(pg_connection&& other);

    awaitable_task<void> init(const pg_config& config, crypto& crypt);

    awaitable_task<std::vector<pg_result>> query(const std::string& q);

    awaitable_task<void> prepare_query(const std::string& q, const pg_params& params);

    awaitable_task<pg_result> execute(long max_rows = 0);

    awaitable_task<void> close_sync();
};

}

#endif
