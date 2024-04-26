#ifndef CPPEVENT_CRYPTO_SHA256_HPP
#define CPPEVENT_CRYPTO_SHA256_HPP

#include <openssl/evp.h>

#include <string>

namespace cppevent {

constexpr int SHA256_OUTPUT_OCTETS = 32;

class crypto;

class sha256 {
private:
    crypto& m_crypt;
    EVP_MD_CTX* m_ctx;

public:
    sha256(crypto& crypt);
    ~sha256();

    void update(const unsigned char* data, long data_len);
    void derive(unsigned char* out, unsigned int* out_l = NULL);
};

}

#endif
