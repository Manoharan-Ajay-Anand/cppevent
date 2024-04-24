#ifndef CPPEVENT_CRYPTO_ENCODING_HPP
#define CPPEVENT_CRYPTO_ENCODING_HPP

#include <cstdint>
#include <vector>
#include <string>
#include <string_view>

namespace cppevent {

std::string base64_encode(const void* data, long size);

std::vector<uint8_t> base64_decode(std::string_view str);

}

#endif
