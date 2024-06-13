#ifndef CPPEVENT_POSTGRES_PG_PARSER_HPP
#define CPPEVENT_POSTGRES_PG_PARSER_HPP

#include <cstdint>
#include <string>
#include <concepts>

namespace cppevent {

int convert_char_num(char c);

template <class T>
struct pg_parser {
    template <class U = T>
    static std::enable_if_t<std::is_unsigned_v<U>, U> parse(const uint8_t* ptr, long size) {
        U val = 0;
        for (long i = 0; i < size; ++i) {
            val = (val * 10) + convert_char_num(*(ptr + i));
        }
        return val;
    }

    template <class U = T>
    static std::enable_if_t<std::signed_integral<U>, U> parse(const uint8_t* ptr, long size) {
        int multiplier = 1;
        U val = 0;
        if (*ptr == '-') {
            multiplier = -1;
            ++ptr;
            --size;
        }
        return parse<std::make_unsigned_t<U>>(ptr, size) * multiplier;
    }

    template <class U = T>
    static std::enable_if_t<std::floating_point<U>, U> parse(const uint8_t* ptr, long size) {
        int multiplier = 1;
        U val = 0;
        if (*ptr == '-') {
            multiplier = -1;
            ++ptr;
            --size;
        }
        long i;
        for (i = 0; i < size && *(ptr + i) != '.'; ++i) {
            val = (val * 10) + convert_char_num(*(ptr + i));
        }
        ++i;
        long divisor = 1;
        for (; i < size; ++i) {
            val = (val * 10) + convert_char_num(*(ptr + i));
            divisor *= 10;
        }
        return (val / divisor) * multiplier;
    }
};

template <>
struct pg_parser<std::string> {
    static std::string parse(const uint8_t* ptr, long size) {
        return { reinterpret_cast<const char*>(ptr), static_cast<size_t>(size) };
    }
};

}

#endif
