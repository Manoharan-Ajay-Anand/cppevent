#ifndef CPPEVENT_POSTGRES_SCRAM_HPP
#define CPPEVENT_POSTGRES_SCRAM_HPP

#include <cppevent_crypto/sha256.hpp>

#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace cppevent {

constexpr char SCRAM_SHA256[] = "SCRAM-SHA-256";

using key_buffer = std::array<uint8_t, SHA256_OUTPUT_OCTETS>;

class scram {
private:
    crypto& m_crypt;

    std::string m_client_first_msg_bare;
    std::string m_client_nonce;

    std::string m_server_first_msg;
    std::string_view m_server_nonce;
    std::vector<uint8_t> m_salt;
    long m_iterations;

    std::string m_expected_server_final_msg;

    key_buffer generate_salted_password(std::string_view password);

    key_buffer generate_hmac(const key_buffer& key, std::string_view input);

    key_buffer generate_sha(const key_buffer& input);
public:
    scram(crypto& crypt);

    std::string generate_client_first_msg(std::string_view user, std::string_view client_nonce);

    void resolve_server_first_msg(const std::string& msg);

    std::string generate_client_final_msg(std::string_view password);

    bool verify_server_final_msg(const std::string& msg);
};

}

#endif
