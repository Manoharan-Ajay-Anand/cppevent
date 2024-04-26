#include "scram.hpp"

#include <cppevent_crypto/crypto.hpp>
#include <cppevent_crypto/encoding.hpp>
#include <cppevent_crypto/hmac.hpp>
#include <cppevent_crypto/pbkdf2.hpp>

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

cppevent::key_buffer cppevent::scram::generate_salted_password(std::string_view password) {
    key_buffer result;

    pbkdf2 pb(m_crypt);
    pb.derive(result.data(), result.size(),
              password.data(), password.size(), m_salt.data(), m_salt.size(),
              m_iterations, SHA256_NAME);
    
    return result;
}

cppevent::key_buffer cppevent::scram::generate_hmac(const key_buffer& key, std::string_view input) {
    key_buffer result;
    size_t out_l;

    hmac hm(m_crypt);
    hm.init(key.data(), key.size(), SHA256_NAME);
    hm.update(reinterpret_cast<const unsigned char*>(input.data()), input.size());
    hm.derive(result.data(), &out_l, result.size());

    return result;
}

cppevent::key_buffer cppevent::scram::generate_sha(const key_buffer& input) {
    key_buffer result;

    sha256 sh(m_crypt);
    sh.update(input.data(), input.size());
    sh.derive(result.data());

    return result;
}

constexpr std::string_view CLIENT_KEY_STR = "Client Key";
constexpr std::string_view SERVER_KEY_STR = "Server Key";

std::string cppevent::scram::generate_client_final_msg(std::string_view password) {
    std::string client_final_msg_bare = std::format("c=biws,r={}", m_server_nonce);

    key_buffer salted_password = generate_salted_password(password);
    
    key_buffer client_key = generate_hmac(salted_password, CLIENT_KEY_STR);

    key_buffer stored_key = generate_sha(client_key);

    std::string auth_message = std::format("{},{},{}", m_client_first_msg_bare,
                                           m_server_first_msg, client_final_msg_bare);

    key_buffer client_signature = generate_hmac(stored_key, auth_message);

    key_buffer client_proof;
    for (long i = 0; i < client_proof.size(); ++i) {
        client_proof[i] = client_key[i] ^ client_signature[i];
    }

    key_buffer server_key = generate_hmac(salted_password, SERVER_KEY_STR);

    key_buffer server_signature = generate_hmac(server_key, auth_message);

    m_expected_server_final_msg = 
            std::format("v={}", base64_encode(server_signature.data(), server_signature.size()));

    return std::format("{},p={}",
                        client_final_msg_bare,
                        base64_encode(client_proof.data(), client_proof.size()));
}

bool cppevent::scram::verify_server_final_msg(const std::string& msg) {
    return msg == m_expected_server_final_msg;
}
