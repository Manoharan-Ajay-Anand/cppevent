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

constexpr int SERVER_FIRST_MSG_PARAMS_COUNT = 3;

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
            start = i;
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

constexpr std::string_view CLIENT_KEY = "Client Key";
constexpr std::string_view SERVER_KEY = "Server Key";

std::string cppevent::scram::generate_client_final_msg(std::string_view password) {
    std::string client_final_msg_bare = std::format("c=biws,r={}", m_server_nonce);

    uint8_t salted_password[SHA256_OUTPUT_OCTETS];
    pbkdf2 pb(m_crypt);
    pb.derive(salted_password, SHA256_OUTPUT_OCTETS, password.data(), password.size(),
              m_salt.data(), m_salt.size(), m_iterations, SHA256_NAME);
    
    uint8_t client_key[SHA256_OUTPUT_OCTETS];
    hmac hm(m_crypt);
    hm.init(salted_password, SHA256_OUTPUT_OCTETS, SHA256_NAME);
    hm.update(reinterpret_cast<const unsigned char*>(CLIENT_KEY.data()), CLIENT_KEY.size());
    size_t out_l;
    hm.derive(client_key, &out_l, SHA256_OUTPUT_OCTETS);

    uint8_t stored_key[SHA256_OUTPUT_OCTETS];
    sha256 sh(m_crypt);
    sh.update(client_key, SHA256_OUTPUT_OCTETS);
    sh.derive(stored_key);

    std::string auth_message = std::format("{},{},{}", m_client_first_msg_bare,
                                           m_server_first_msg, client_final_msg_bare);

    uint8_t client_signature[SHA256_OUTPUT_OCTETS];
    hm.init(stored_key, SHA256_OUTPUT_OCTETS, SHA256_NAME);
    hm.update(reinterpret_cast<unsigned char*>(auth_message.data()), auth_message.size());
    hm.derive(client_signature, &out_l, SHA256_OUTPUT_OCTETS);

    uint8_t client_proof[SHA256_OUTPUT_OCTETS];
    for (long i = 0; i < SHA256_OUTPUT_OCTETS; ++i) {
        client_proof[i] = client_key[i] ^ client_signature[i];
    }

    uint8_t server_key[SHA256_OUTPUT_OCTETS];
    hm.init(salted_password, SHA256_OUTPUT_OCTETS, SHA256_NAME);
    hm.update(reinterpret_cast<const unsigned char*>(SERVER_KEY.data()), SERVER_KEY.size());
    hm.derive(server_key, &out_l, SHA256_OUTPUT_OCTETS);

    uint8_t server_signature[SHA256_OUTPUT_OCTETS];
    hm.init(salted_password, SHA256_OUTPUT_OCTETS, SHA256_NAME);
    hm.update(reinterpret_cast<unsigned char*>(auth_message.data()), auth_message.size());
    hm.derive(server_signature, &out_l, SHA256_OUTPUT_OCTETS);

    m_expected_server_final_msg = std::format("v={}", 
                                              base64_encode(server_signature, SHA256_OUTPUT_OCTETS));
    return std::format("{},p={}", client_final_msg_bare,
                                  base64_encode(client_proof, SHA256_OUTPUT_OCTETS));
}

bool cppevent::scram::verify_server_final_msg(const std::string& msg) {
    return msg == m_expected_server_final_msg;
}
