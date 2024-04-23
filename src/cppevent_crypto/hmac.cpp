#include "hmac.hpp"

#include "crypto.hpp"

#include <stdexcept>

cppevent::hmac::hmac(crypto& crypt): m_crypt(crypt) {
    m_ctx = EVP_MAC_CTX_new(m_crypt.fetch_mac("HMAC"));
}

cppevent::hmac::~hmac() {
    EVP_MAC_CTX_free(m_ctx);
}

void cppevent::hmac::init(const unsigned char* key, long key_len, std::string digest) {
    const OSSL_PARAM params[] = {
        OSSL_PARAM_utf8_string("digest", digest.data(), digest.size()),
        OSSL_PARAM_END
    };
    int status = EVP_MAC_init(m_ctx, key, key_len, params);
    if (status != 1) {
        throw std::runtime_error("hmac init error");
    }
}

void cppevent::hmac::update(const unsigned char* data, long data_len) {
    int status = EVP_MAC_update(m_ctx, data, data_len);
    if (status != 1) {
        throw std::runtime_error("hmac update error");
    }
}

void cppevent::hmac::derive(unsigned char* out, size_t* out_l, long out_size) {
    int status = EVP_MAC_final(m_ctx, out, out_l, out_size);
    if (status != 1) {
        throw std::runtime_error("hmac derive error");
    }
}

