#include "crypto.hpp"

#include <openssl/kdf.h>

#include <stdexcept>

cppevent::crypto::~crypto() {
    for (auto& p : m_md_algos) {
        EVP_MD_free(p.second);
    }
    for (auto& p : m_kdf_algos) {
        EVP_KDF_free(p.second);
    }
    for (auto& p : m_mac_algos) {
        EVP_MAC_free(p.second);
    }
}

EVP_MD* cppevent::crypto::fetch_md(const char* algo) {
    std::string_view algo_view { algo };
    auto it = m_md_algos.find(algo_view);
    if (it != m_md_algos.end()) {
        return it->second;
    }
    EVP_MD* md = EVP_MD_fetch(NULL, algo, NULL);
    if (md == NULL) {
        throw std::runtime_error("Failed to fetch md");
    }
    m_md_algos[algo_view] = md;
    return md;
}

EVP_KDF* cppevent::crypto::fetch_kdf(const char* algo) {
    std::string_view algo_view { algo };
    auto it = m_kdf_algos.find(algo_view);
    if (it != m_kdf_algos.end()) {
        return it->second;
    }
    EVP_KDF* kdf = EVP_KDF_fetch(NULL, algo, NULL);
    if (kdf == NULL) {
        throw std::runtime_error("Failed to fetch kdf");
    }
    m_kdf_algos[algo_view] = kdf;
    return kdf;
}

EVP_MAC* cppevent::crypto::fetch_mac(const char* algo) {
    std::string_view algo_view { algo };
    auto it = m_mac_algos.find(algo_view);
    if (it != m_mac_algos.end()) {
        return it->second;
    }
    EVP_MAC* mac = EVP_MAC_fetch(NULL, algo, NULL);
    if (mac == NULL) {
        throw std::runtime_error("Failed to fetch mac");
    }
    m_mac_algos[algo_view] = mac;
    return mac;
}
