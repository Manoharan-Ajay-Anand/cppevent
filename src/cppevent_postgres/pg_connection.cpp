#include "pg_connection.hpp"

#include "types.hpp"
#include "scram.hpp"

#include <cppevent_base/util.hpp>

#include <cppevent_crypto/crypto.hpp>
#include <cppevent_crypto/random.hpp>
#include <cppevent_crypto/encoding.hpp>

#include <array>
#include <string>
#include <format>
#include <stdexcept>
#include <iostream>

constexpr int STARTUP_HEADER_SIZE = 8;
constexpr uint32_t POSTGRES_PROTOCOL_MAJOR_VERSION = 3;
constexpr uint32_t POSTGRES_PROTOCOL_MINOR_VERSION = 0;
constexpr uint32_t POSTGRES_PROTOCOL = (POSTGRES_PROTOCOL_MAJOR_VERSION << 16) + 
                                        POSTGRES_PROTOCOL_MINOR_VERSION;

constexpr char STARTUP_PARAM_USER[] = "user";
constexpr char STARTUP_PARAM_DATABASE[] = "database";

constexpr int HEADER_SIZE = 5;

constexpr int INT_32_OCTETS = 4;

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

class response_header {
private:
    uint8_t m_buf[HEADER_SIZE];
public:
    void set_type(char c) {
        m_buf[0] = c;
    }

    void set_size(long size) {
        cppevent::write_u32_be(&(m_buf[1]), size + INT_32_OCTETS);
    }

    const uint8_t* data() {
        return m_buf;
    }

    constexpr long size() {
        return HEADER_SIZE;
    }
};

constexpr long CLIENT_NONCE_OCTETS = 24;

cppevent::awaitable_task<void> cppevent::pg_connection::handle_auth(response_info info,
                                                                    const pg_config& config,
                                                                    scram& scr) {
    uint8_t type_data[INT_32_OCTETS];
    co_await m_sock->read(type_data, INT_32_OCTETS, true);

    auth_type type = static_cast<auth_type>(read_u32_be(type_data));
    long msg_size = info.m_size - INT_32_OCTETS;

    response_header res_header;
    res_header.set_type('p');

    switch (type) {
        case auth_type::OK:
            break;
        case auth_type::CLEAR_TEXT_PASSWORD: {
            long password_size = config.m_password.size() + 1;
            res_header.set_size(password_size);
            co_await m_sock->write(res_header.data(), res_header.size());
            co_await m_sock->write(config.m_password.c_str(), password_size);
            co_await m_sock->flush();
            break;
        }
        case auth_type::SASL: {
            std::string sasl_mechanisms;
            co_await m_sock->read(sasl_mechanisms, msg_size, true);
            if (sasl_mechanisms.find(SCRAM_SHA256) == sasl_mechanisms.npos) {
                throw std::runtime_error("SCRAM mechanism not found");
            }

            std::string c_nonce = generate_random_string<CLIENT_NONCE_OCTETS>(base64_encode);
            std::string client_first_msg = scr.generate_client_first_msg(config.m_user, c_nonce);

            res_header.set_size(sizeof(SCRAM_SHA256) + INT_32_OCTETS + client_first_msg.size());

            co_await m_sock->write(res_header.data(), res_header.size());
            co_await m_sock->write(SCRAM_SHA256, sizeof(SCRAM_SHA256));
            
            uint8_t arr[INT_32_OCTETS];
            write_u32_be(arr, client_first_msg.size());

            co_await m_sock->write(arr, INT_32_OCTETS);
            co_await m_sock->write(client_first_msg.data(), client_first_msg.size());
            co_await m_sock->flush();
            break;
        }
        case auth_type::SASL_CONTINUE: {
            std::string server_first_msg;
            co_await m_sock->read(server_first_msg, msg_size, true);
            scr.resolve_server_first_msg(server_first_msg);

            std::string client_final_msg = scr.generate_client_final_msg(config.m_password);
            res_header.set_size(client_final_msg.size());

            co_await m_sock->write(res_header.data(), res_header.size());
            co_await m_sock->write(client_final_msg.data(), client_final_msg.size());
            co_await m_sock->flush();
            break;
        }
        case auth_type::SASL_FINAL: {
            std::string server_final_msg;
            co_await m_sock->read(server_final_msg, msg_size, true);
            if (!scr.verify_server_final_msg(server_final_msg)) {
                throw std::runtime_error("SASL Auth unexpected final msg");
            }
            break;
        }
        default:
            throw std::runtime_error("Unrecognized auth method");
    }
}

cppevent::awaitable_task<void> cppevent::pg_connection::init(std::unique_ptr<socket>&& sock,
                                                             long* conn_count,
                                                             const pg_config& config,
                                                             crypto& crypt) {
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
    write_u32_be(&(header[4]), POSTGRES_PROTOCOL);

    co_await m_sock->write(header, STARTUP_HEADER_SIZE);
    co_await m_sock->write(message.data(), message.size());
    co_await m_sock->flush();

    bool query_ready = false;
    scram scr(crypt);

    while (!query_ready) {
        response_info info = co_await get_response_info(); 

        switch (info.m_type) {
            case response_type::ERROR_RESPONSE:
                throw std::runtime_error("Postgres ErrorResponse");
            case response_type::NEGOTIATE_PROTOCOL_VERSION:
                throw std::runtime_error("Postgres Protocol Version mismatch");
            case response_type::AUTHENTICATION:
                co_await handle_auth(info, config, scr);
                break;
            case response_type::BACKEND_KEY_DATA: {
                std::array<uint8_t, 2 * INT_32_OCTETS> arr;
                co_await m_sock->read(arr.data(), arr.size(), true);
                break;
            }
            case response_type::PARAMETER_STATUS: {
                std::string content;
                co_await m_sock->read(content, info.m_size, true);
                break;
            }
            case response_type::READY_FOR_QUERY: {
                uint8_t status = co_await m_sock->read_c(true);
                query_ready = true;
                break;
            }
            default:
                throw std::runtime_error("Postgres unexpected response");
        }
    }
}
