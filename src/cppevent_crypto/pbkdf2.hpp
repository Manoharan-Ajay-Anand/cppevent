#ifndef CPPEVENT_CRYPTO_PBKDF2_HPP
#define CPPEVENT_CRYPTO_PBKDF2_HPP

#include <openssl/kdf.h>

#include <string_view>
#include <cstdint>

namespace cppevent {

class crypto;

class pbkdf2 {
private:
    crypto& m_crypt;
    EVP_KDF_CTX* m_ctx;

public:
    pbkdf2(crypto& crypt);
    ~pbkdf2();
    
    void derive(unsigned char* out, long out_len,
                const void* pass, long pass_len,
                const void* salt, long salt_len,
                uint64_t iter, std::string_view digest);
    
    void reset();
};

}

#endif
