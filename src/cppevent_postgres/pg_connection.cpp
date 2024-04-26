#include "pg_connection.hpp"

#include "types.hpp"

#include <cppevent_base/util.hpp>
#include <cppevent_crypto/encoding.hpp>

#include <format>
#include <stdexcept>

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

constexpr std::string_view GS2_HEADER = "n,,";

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

constexpr long SERVER_FIRST_MSG_PARAMS_COUNT = 3;

long convert_to_num(std::string_view input) {
    long result = 0;
    for (char c : input) {
        result = result * 10 + (c - '0');
    }
    return result;
}

void resolve_server_first_msg(cppevent::sasl_context& context) {
    std::string& server_first_msg = context.m_server_first_msg;
    long start = 2;
    std::vector<std::string_view> params;
    for (long i = start; i <= server_first_msg.size();) {
        if (i == server_first_msg.size() || server_first_msg[i] == ',') {
            params.push_back(std::string_view { server_first_msg.data() + start, i - start });
            i += 3;
        } else {
            ++i;
        }
    }
    if (params.size() != SERVER_FIRST_MSG_PARAMS_COUNT) {
        throw std::runtime_error("Not enough params server first message");
    }
    if (!params[0].starts_with(context.m_client_nonce)) {
        throw std::runtime_error("Server nonce doesn't begin with client nonce");
    }
    context.m_server_nonce = params[0];
    context.m_salt = cppevent::base64_decode(params[1]);
    context.m_iterations = convert_to_num(params[2]);
}

cppevent::awaitable_task<bool> cppevent::pg_connection::handle_auth(response_info info,
                                                                    const pg_config& config,
                                                                    sasl_context& context) {
    uint8_t type_data[INT_32_OCTETS];
    co_await m_sock->read(type_data, INT_32_OCTETS, true);
    auth_type type = static_cast<auth_type>(read_u32_be(type_data));

    uint8_t msg_header[HEADER_SIZE];
    msg_header[0] = 'p';

    switch (type) {
        case auth_type::OK:
            co_return true;
        case auth_type::CLEAR_TEXT_PASSWORD: {
            long password_size = config.m_password.size() + 1;
            write_u32_be(&(msg_header[1]), password_size + INT_32_OCTETS);
            co_await m_sock->write(msg_header, HEADER_SIZE);
            co_await m_sock->write(config.m_password.c_str(), password_size);
            co_await m_sock->flush();
            break;
        }
        case auth_type::SASL: {
            std::string sasl_mechanisms;
            co_await m_sock->read(sasl_mechanisms, info.m_size - INT_32_OCTETS, true);
            if (sasl_mechanisms.find(SCRAM_SHA256) == sasl_mechanisms.npos) {
                throw std::runtime_error("SCRAM mechanism not found");
            }

            std::string& client_nonce = context.m_client_nonce;
            std::string& client_first_msg_bare = context.m_client_first_msg_bare;

            client_nonce = generate_client_nonce();
            client_first_msg_bare = std::format("n={},r={}", config.m_user, client_nonce);
            long client_first_msg_size = GS2_HEADER.size() + client_first_msg_bare.size();

            long msg_size = 2* INT_32_OCTETS + sizeof(SCRAM_SHA256) + client_first_msg_size;
            write_u32_be(&(msg_header[1]), msg_size);

            co_await m_sock->write(msg_header, HEADER_SIZE);
            co_await m_sock->write(SCRAM_SHA256, sizeof(SCRAM_SHA256));
            
            uint8_t arr[INT_32_OCTETS];
            write_u32_be(arr, client_first_msg_size);

            co_await m_sock->write(arr, INT_32_OCTETS);
            co_await m_sock->write(GS2_HEADER.data(), GS2_HEADER.size());
            co_await m_sock->write(client_first_msg_bare.data(), client_first_msg_bare.size());
            co_await m_sock->flush();
            break;
        }
        case auth_type::SASL_CONTINUE: {
            co_await m_sock->read(context.m_server_first_msg, info.m_size - 2 * INT_32_OCTETS, true);
            resolve_server_first_msg(context);
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
    sasl_context context;

    while (!auth_success) {
        response_info info = co_await get_response_info(); 

        switch (info.m_type) {
            case response_type::ERROR_RESPONSE:
                throw std::runtime_error("Postgres ErrorResponse");
            case response_type::NEGOTIATE_PROTOCOL_VERSION:
                throw std::runtime_error("Postgres Protocol Version mismatch");
            case response_type::AUTHENTICATION:
                auth_success = co_await handle_auth(info, config, context);
                break;
            default:
                throw std::runtime_error("Postgres unexpected response");
        }
    }
}
