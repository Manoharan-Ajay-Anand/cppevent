#ifndef CPPEVENT_CRYPTO_CRYPTO_HPP
#define CPPEVENT_CRYPTO_CRYPTO_HPP

#include <openssl/evp.h>

#include <string_view>
#include <unordered_map>

namespace cppevent {

class crypto {
private:
    std::unordered_map<std::string_view, EVP_MD*> m_md_algos;
    std::unordered_map<std::string_view, EVP_KDF*> m_kdf_algos;
    std::unordered_map<std::string_view, EVP_MAC*> m_mac_algos;
public:
    ~crypto();

    EVP_MD* fetch_md(const char* algo);
    EVP_KDF* fetch_kdf(const char* algo);
    EVP_MAC* fetch_mac(const char* algo);
};

}

#endif
