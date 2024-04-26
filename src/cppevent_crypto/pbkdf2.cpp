#include "pbkdf2.hpp"

#include "crypto.hpp"

#include <stdexcept>

cppevent::pbkdf2::pbkdf2(crypto& crypt): m_crypt(crypt) {
    m_ctx = EVP_KDF_CTX_new(m_crypt.fetch_kdf("PBKDF2"));
}

cppevent::pbkdf2::~pbkdf2() {
    EVP_KDF_CTX_free(m_ctx);
}

void cppevent::pbkdf2::derive(unsigned char* out, long out_len,
                              const void* pass, long pass_len,
                              const void* salt, long salt_len,
                              uint64_t iter, std::string_view digest) {
    const char* digest_ptr = digest.data();
    const OSSL_PARAM params[] = {
        OSSL_PARAM_octet_ptr("pass", &pass, pass_len),
        OSSL_PARAM_octet_ptr("salt", &salt, salt_len),
        OSSL_PARAM_uint64("iter", &iter),
        OSSL_PARAM_utf8_ptr("digest", &digest_ptr, digest.size()),
        OSSL_PARAM_END
    };
    int status = EVP_KDF_derive(m_ctx, out, out_len, params);
    if (status != 1) {
        throw std::runtime_error("pbkdf2 derive error");
    }
}

void cppevent::pbkdf2::reset() {
    EVP_KDF_CTX_reset(m_ctx);
}
