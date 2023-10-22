#ifndef CPPEVENT_BASE_UTIL_HPP
#define CPPEVENT_BASE_UTIL_HPP

#include <string>
#include <string_view>
#include <cstdint>

namespace cppevent {

void throw_error(std::string prefix, int errnum);

void throw_error(std::string_view prefix);

void throw_if_error(int status, std::string_view prefix);

uint16_t read_u16_be(const void* src);

uint32_t read_u32_be(const void* src);

void write_u16_be(void* dest, uint16_t val);

void write_u32_be(void* dest, uint32_t val);

}

#endif
