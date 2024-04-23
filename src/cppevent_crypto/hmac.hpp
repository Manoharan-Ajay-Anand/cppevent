#ifndef CPPEVENT_CRYPTO_HMAC_HPP
#define CPPEVENT_CRYPTO_HMAC_HPP

#include <openssl/evp.h>

#include <string>

namespace cppevent {

class crypto;

class hmac {
private:
    crypto& m_crypt;
    EVP_MAC_CTX* m_ctx;

public:
    hmac(crypto& crypt);
    ~hmac();

    void init(const unsigned char* key, long key_len, std::string digest);
    void update(const unsigned char* data, long data_len);
    void derive(unsigned char* out, size_t* out_l, long out_size);
};

}

#endif
