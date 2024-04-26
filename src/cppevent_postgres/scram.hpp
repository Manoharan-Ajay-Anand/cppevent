#ifndef CPPEVENT_POSTGRES_SCRAM_HPP
#define CPPEVENT_POSTGRES_SCRAM_HPP

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace cppevent {

class crypto;

constexpr char SCRAM_SHA256[] = "SCRAM-SHA-256";

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
public:
    scram(crypto& crypt);

    std::string generate_client_first_msg(std::string_view user);

    void resolve_server_first_msg(const std::string& msg);

    std::string generate_client_final_msg(std::string_view password);

    bool verify_server_final_msg(const std::string& msg);
};

}

#endif
