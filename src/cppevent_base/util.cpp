#include "util.hpp"

#include <cerrno>
#include <stdexcept>
#include <cstring>

void cppevent::throw_error(std::string prefix, int errnum) {
    throw std::runtime_error(prefix.append(strerror(errnum)));
}

void cppevent::throw_error(std::string_view prefix) {
    return throw_error({ prefix.begin(), prefix.end() }, errno);
}

void cppevent::throw_if_error(int status, std::string_view prefix) {
    if (status < 0) {
        throw_error(prefix);
    }
}

uint16_t cppevent::read_u16_be(const void* src) {
    const uint8_t* ptr = static_cast<const uint8_t*>(src);
    return (static_cast<uint16_t>(*ptr) << 8) + *(ptr + 1); 
}

uint32_t cppevent::read_u32_be(const void* src) {
    const uint8_t* ptr = static_cast<const uint8_t*>(src);
    return (static_cast<uint32_t>(read_u16_be(ptr)) << 16) + read_u16_be(ptr + 2);
}

void cppevent::write_u16_be(void* dest, uint16_t val) {
    uint8_t* ptr = static_cast<uint8_t*>(dest);
    *ptr = (val >> 8) & 0xFF;
    *(ptr + 1) = val & 0xFF;
}

void cppevent::write_u32_be(void* dest, uint32_t val) {
    uint8_t* ptr = static_cast<uint8_t*>(dest);
    write_u16_be(ptr, (val >> 16) & 0xFFFF);
    write_u16_be(ptr + 2, val & 0xFFFF);
}