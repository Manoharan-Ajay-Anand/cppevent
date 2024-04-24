#include "encoding.hpp"

#include <stdexcept>

constexpr int OCTET = 8;

constexpr int BASE64_BITS = 6;

constexpr int BASE64_PADDING = 2;

constexpr char base64_enc(uint8_t b) {
    switch (b) {
        case 0 ... 25:
            return 'A' + b;
        case 26 ... 51:
            return 'a' + b - 26;
        case 52 ... 61:
            return '0' + b - 52;
        case 62:
            return '+';
        case 63:
            return '/';
        default:
            throw std::runtime_error("base64 encode out of range");
    }
}

constexpr uint8_t base64_dec(char c) {
    switch (c) {
        case 'A' ... 'Z':
            return c - 'A';
        case 'a' ... 'z':
            return c - 'a' + 26;
        case '0' ... '9':
            return c - '0' + 52;
        case '+':
            return 62;
        case '/':
            return 63;
        default:
            throw std::runtime_error("unrecognized character base64");
    }
}

template <size_t NUM_BITS>
uint64_t generate_mask() {
    return (generate_mask<NUM_BITS - 1>() << 1) + 1;
}

template <>
uint64_t generate_mask<0>() {
    return 0;
}

template <size_t MSB_BITS>
uint64_t get_msb(uint64_t num, long total_bits) {
    uint64_t mask = generate_mask<MSB_BITS>();
    return (num >> (total_bits - MSB_BITS)) & mask;
}

std::string cppevent::base64_encode(const void* data, long size) {
    const uint8_t* data_ptr = static_cast<const uint8_t*>(data);
    std::string result;
    uint64_t num = 0;
    long num_bits = 0;
    for (long i = 0; i < size; ++i) {
        uint8_t b = *(data_ptr + i);
        num = (num << OCTET) + b;
        num_bits += OCTET;

        if (num_bits % BASE64_BITS != 0) continue; 
        
        for (; num_bits > 0; num_bits -= BASE64_BITS) {
            result.push_back(base64_enc(get_msb<BASE64_BITS>(num, num_bits)));
        }
    }

    int padding_bits = 0;
    switch (num_bits) {
        case OCTET:
            padding_bits = 2 * BASE64_PADDING;
            break;
        case 2 * OCTET:
            padding_bits = BASE64_PADDING;
            break;
    }
    
    num = num << padding_bits;
    num_bits += padding_bits;
    for (; num_bits > 0; num_bits -= BASE64_BITS) {
        result.push_back(base64_enc(get_msb<BASE64_BITS>(num, num_bits)));
    }
    result.append(padding_bits / BASE64_PADDING, '=');
    return result;
}

std::vector<uint8_t> cppevent::base64_decode(std::string_view str) {
    std::vector<uint8_t> result;
    uint64_t num = 0;
    long num_bits = 0;
    for (char c : str) {
        if (c != '=') {
            num = (num << BASE64_BITS) + base64_dec(c);
            num_bits += BASE64_BITS;
        } else {
            num = num >> BASE64_PADDING;
            num_bits -= BASE64_PADDING;
        }

        if (num_bits % OCTET != 0) continue; 
        
        for (; num_bits > 0; num_bits -= OCTET) {
            result.push_back(get_msb<OCTET>(num, num_bits));
        }
    }
    return result;
}
