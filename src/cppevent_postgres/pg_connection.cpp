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
constexpr int INT_16_OCTETS = 2;

cppevent::pg_connection::pg_connection(std::unique_ptr<socket>&& sock,
                                       long* conn_count): m_sock(std::move(sock)),
                                                          m_conn_count(conn_count) {
    ++(*m_conn_count);
}

cppevent::pg_connection::~pg_connection() {
    if (m_sock) {
        --(*m_conn_count);
    }
}

cppevent::pg_connection::pg_connection(pg_connection&& other) {
    m_sock = std::move(other.m_sock);
    m_conn_count = other.m_conn_count;
    m_query_ready = other.m_query_ready;
}

cppevent::pg_connection& cppevent::pg_connection::operator=(pg_connection&& other) {
    m_sock = std::move(other.m_sock);
    m_conn_count = other.m_conn_count;
    m_query_ready = other.m_query_ready;
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

cppevent::awaitable_task<void> cppevent::pg_connection::init(const pg_config& config,
                                                             crypto& crypt) {
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

    scram scr(crypt);

    while (!m_query_ready) {
        response_info info = co_await get_response_info(); 

        switch (info.m_type) {
            case response_type::ERROR_RESPONSE:
                throw std::runtime_error("Postgres ErrorResponse");
            case response_type::NEGOTIATE_PROTOCOL_VERSION:
                throw std::runtime_error("Postgres Protocol Version mismatch");
            case response_type::AUTHENTICATION:
                co_await handle_auth(info, config, scr);
                break;
            case response_type::BACKEND_KEY_DATA:
            case response_type::PARAMETER_STATUS:
                co_await m_sock->skip(info.m_size, true);
                break;
            case response_type::READY_FOR_QUERY: {
                co_await m_sock->skip(info.m_size, true);
                m_query_ready = true;
                break;
            }
            default:
                throw std::runtime_error("Postgres unexpected response");
        }
    }
}

cppevent::awaitable_task<void> cppevent::pg_connection::query(const std::string& q) {
    while (!m_query_ready) {
        response_info info = co_await get_response_info();
        co_await m_sock->skip(info.m_size, true);
        if (info.m_type == response_type::READY_FOR_QUERY) {
            m_query_ready = true;
        }
    }

    response_header res_header;
    res_header.set_type('Q');

    const long query_len = q.size() + 1;
    res_header.set_size(query_len);

    co_await m_sock->write(res_header.data(), res_header.size());
    co_await m_sock->write(q.c_str(), query_len);
    co_await m_sock->flush();

    m_query_ready = false;
}

cppevent::awaitable_task<cppevent::pg_result> cppevent::pg_connection::get_result() {
    pg_result result;
    if (m_query_ready) {
        throw std::runtime_error("pg_connection get_result: no query specified");
    }
    while (result.get_type() == result_type::PENDING) {
        response_info info = co_await get_response_info();
        switch (info.m_type) {
            case response_type::ROW_DESCRIPTION: {
                std::vector<uint8_t> desc_data(info.m_size);
                uint8_t* data_p = desc_data.data();
                result.set_desc_data(std::move(desc_data));
                co_await m_sock->read(data_p, info.m_size, true);

                int num_cols = read_u16_be(data_p);
                data_p += INT_16_OCTETS;
                for (int i = 0; i < num_cols; ++i) {
                    int col_name_len = 0;
                    for (; *(data_p + col_name_len) != 0; ++col_name_len) {
                    }
                    std::string_view col_name = { reinterpret_cast<char*>(data_p),
                                                  static_cast<size_t>(col_name_len) };
                    data_p += col_name_len + 1;
                    data_p += 4 * INT_32_OCTETS;
                    format_code col_code = static_cast<format_code>(read_u16_be(data_p));
                    data_p += INT_16_OCTETS;
                    result.add_column({ col_name, col_code });
                }
                break;
            }
            case response_type::DATA_ROW: {
                std::vector<uint8_t> row_data(info.m_size);
                uint8_t* data_p = row_data.data();
                result.add_row_data(std::move(row_data));
                co_await m_sock->read(data_p, info.m_size, true);

                std::vector<pg_value> row;
                int num_cols = read_u16_be(data_p);
                data_p += INT_16_OCTETS;
                for (int i = 0; i < num_cols; ++i) {
                    int32_t val_len = static_cast<int32_t>(read_u32_be(data_p));
                    data_p += INT_32_OCTETS;
                    row.push_back(pg_value { data_p, val_len });
                    if (val_len > 0) data_p += val_len;
                }
                result.add_row(std::move(row));
                break;
            }
            case response_type::COMMAND_COMPLETE: {
                std::string tag;
                co_await m_sock->read(tag, info.m_size, true);
                tag.pop_back();
                result.set_command_tag(std::move(tag));
                break;
            }
            case response_type::EMPTY_QUERY_RESPONSE:
            case response_type::ERROR_RESPONSE:
                result.set_error();
            case response_type::NOTICE_RESPONSE:
                m_sock->skip(info.m_size, true);
                break;
            default:
                throw std::runtime_error("pg_connection get_result: unknown response type");
        }
    }
    co_return std::move(result);
}
