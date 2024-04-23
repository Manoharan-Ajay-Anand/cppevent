#include "sha256.hpp"

#include "crypto.hpp"

#include <stdexcept>

cppevent::sha256::sha256(crypto& crypt): m_crypt(crypt) {
    m_ctx = EVP_MD_CTX_new();
    if (EVP_DigestInit(m_ctx, m_crypt.fetch_md("SHA256")) != 1) {
        throw std::runtime_error("sha25g constructor error");
    }
}

cppevent::sha256::~sha256() {
    EVP_MD_CTX_free(m_ctx);
}

void cppevent::sha256::update(const unsigned char* data, long data_len) {
    if (EVP_DigestUpdate(m_ctx, data, data_len) != 1) {
        throw std::runtime_error("sha256 update error");
    }
}

void cppevent::sha256::derive(unsigned char* out, unsigned int* out_l) {
    if (EVP_DigestFinal(m_ctx, out, out_l) != 1) {
        throw std::runtime_error("sha256 derive error");
    }
}
