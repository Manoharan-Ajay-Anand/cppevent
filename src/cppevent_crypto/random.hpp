#ifndef CPPEVENT_CRYPTO_RANDOM_HPP
#define CPPEVENT_CRYPTO_RANDOM_HPP

#include <array>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <string>

#include <openssl/rand.h>

namespace cppevent {

template <long BYTES_SIZE, typename func>
std::string generate_random_string(func&& encode_fn) {
    std::array<uint8_t, BYTES_SIZE> input;
    if (RAND_bytes(input.data(), input.size()) != 1) {
        throw std::runtime_error("Error generating RAND_BYTES");
    }
    return encode_fn(input.data(), input.size());
}

}

#endif
