#include "pg_connection.hpp"

#include "types.hpp"

#include <cppevent_base/util.hpp>
#include <cppevent_crypto/encoding.hpp>

#include <string>
#include <string_view>
#include <stdexcept>
#include <unordered_set>

#include <openssl/rand.h>

constexpr long STARTUP_HEADER_SIZE = 8;
constexpr uint32_t POSTGRES_PROTOCOL_MAJOR_VERSION = 3;
constexpr uint32_t POSTGRES_PROTOCOL_MINOR_VERSION = 0;
constexpr uint32_t POSTGRES_PROTOCOL = (POSTGRES_PROTOCOL_MAJOR_VERSION << 16) + 
                                        POSTGRES_PROTOCOL_MINOR_VERSION;

constexpr char STARTUP_PARAM_USER[] = "user";
constexpr char STARTUP_PARAM_DATABASE[] = "database";

constexpr long HEADER_SIZE = 5;

constexpr long INT_32_OCTETS = 4;

constexpr char SCRAM_SHA256[] = "SCRAM-SHA-256";

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

cppevent::awaitable_task<cppevent::response_info> cppevent::pg_connection::get_response_info() {
    uint8_t res_header[HEADER_SIZE];
    co_await m_sock->read(res_header, HEADER_SIZE, true);

    response_type type = static_cast<response_type>(res_header[0]);
    long size = read_u32_be(&(res_header[1])) - INT_32_OCTETS;

    co_return response_info { type, size };
}

constexpr long CLIENT_NONCE_OCTETS = 24;

std::string generate_client_nonce() {
    uint8_t nonce_data[CLIENT_NONCE_OCTETS];
    if (RAND_bytes(nonce_data, CLIENT_NONCE_OCTETS) != 1) {
        throw std::runtime_error("Error generating RAND_BYTES");
    }
    return cppevent::base64_encode(nonce_data, CLIENT_NONCE_OCTETS);
}

cppevent::awaitable_task<bool> cppevent::pg_connection::handle_auth(response_info info,
                                                                    const pg_config& config) {
    uint8_t type_data[INT_32_OCTETS];
    co_await m_sock->read(type_data, INT_32_OCTETS, true);
    auth_type type = static_cast<auth_type>(read_u32_be(type_data));
    switch (type) {
        case auth_type::OK:
            co_return true;
        case auth_type::CLEAR_TEXT_PASSWORD: {
            uint8_t msg_header[HEADER_SIZE];
            msg_header[0] = 'p';
            long password_size = config.m_password.size() + 1;
            write_u32_be(&(msg_header[1]), password_size + INT_32_OCTETS);
            co_await m_sock->write(msg_header, HEADER_SIZE);
            co_await m_sock->write(config.m_password.c_str(), password_size);
            break;
        }
        case auth_type::SASL: {
            std::string sasl_mechanisms;
            co_await m_sock->read(sasl_mechanisms, info.m_size - INT_32_OCTETS, true);
            if (sasl_mechanisms.find(SCRAM_SHA256) == sasl_mechanisms.npos) {
                throw std::runtime_error("SCRAM mechanism not found");
            }
            break;
        }
        default:
            throw std::runtime_error("Unrecognized auth method");
    }
    co_return false;
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

    bool auth_success = false;

    while (!auth_success) {
        response_info info = co_await get_response_info(); 

        switch (info.m_type) {
            case response_type::ERROR_RESPONSE:
                throw std::runtime_error("Postgres ErrorResponse");
            case response_type::NEGOTIATE_PROTOCOL_VERSION:
                throw std::runtime_error("Postgres Protocol Version mismatch");
            case response_type::AUTHENTICATION:
                auth_success = co_await handle_auth(info, config);
                break;
            default:
                throw std::runtime_error("Postgres unexpected response");
        }
    }
}
