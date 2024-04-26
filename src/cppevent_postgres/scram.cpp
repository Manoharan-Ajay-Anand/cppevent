#include "scram.hpp"

#include <cppevent_crypto/crypto.hpp>
#include <cppevent_crypto/encoding.hpp>
#include <cppevent_crypto/hmac.hpp>
#include <cppevent_crypto/pbkdf2.hpp>
#include <cppevent_crypto/sha256.hpp>

#include <openssl/rand.h>

#include <format>

constexpr char GS2_HEADER[] = "n,,";

cppevent::scram::scram(crypto& crypt): m_crypt(crypt) {
}

constexpr long CLIENT_NONCE_OCTETS = 24;

std::string generate_client_nonce() {
    uint8_t nonce_data[CLIENT_NONCE_OCTETS];
    if (RAND_bytes(nonce_data, CLIENT_NONCE_OCTETS) != 1) {
        throw std::runtime_error("Error generating RAND_BYTES");
    }
    return cppevent::base64_encode(nonce_data, CLIENT_NONCE_OCTETS);
}

std::string cppevent::scram::generate_client_first_msg(std::string_view user) {
    m_client_nonce = generate_client_nonce();
    m_client_first_msg_bare = std::format("n={},r={}", user, m_client_nonce);
    return std::format("{}{}", GS2_HEADER, m_client_first_msg_bare);
}

constexpr long SERVER_FIRST_MSG_PARAMS_COUNT = 3;

long convert_to_num(std::string_view input) {
    long result = 0;
    for (char c : input) {
        result = result * 10 + (c - '0');
    }
    return result;
}

void cppevent::scram::resolve_server_first_msg(const std::string& msg) {
    m_server_first_msg = msg;

    long start = 2;
    std::vector<std::string_view> params;
    for (long i = start; i <= m_server_first_msg.size();) {
        if (i == m_server_first_msg.size() || m_server_first_msg[i] == ',') {
            params.push_back(std::string_view { m_server_first_msg.data() + start, i - start });
            i += 3;
        } else {
            ++i;
        }
    }

    if (params.size() != SERVER_FIRST_MSG_PARAMS_COUNT) {
        throw std::runtime_error("Not enough params server first message");
    }
    if (!params[0].starts_with(m_client_nonce)) {
        throw std::runtime_error("Server nonce doesn't begin with client nonce");
    }
    
    m_server_nonce = params[0];
    m_salt = cppevent::base64_decode(params[1]);
    m_iterations = convert_to_num(params[2]);
}
