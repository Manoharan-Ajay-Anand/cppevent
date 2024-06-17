#ifndef CPPEVENT_POSTGRES_PG_PARSER_HPP
#define CPPEVENT_POSTGRES_PG_PARSER_HPP

#include <cppevent_base/util.hpp>

#include <array>
#include <cstdint>
#include <charconv>
#include <string>
#include <concepts>
#include <stdexcept>
#include <vector>

namespace cppevent {

template <class T>
concept is_number = std::integral<T> || std::floating_point<T>; 

template <class T>
struct pg_parser {
    template <class U = T>
    static std::enable_if_t<is_number<U>, U> parse(const uint8_t* ptr, long size) {
        U val;
        const char* c_ptr = reinterpret_cast<const char*>(ptr);
        std::from_chars_result result = std::from_chars(c_ptr, c_ptr + size, val);
        if (result.ec == std::errc::result_out_of_range) {
            throw std::runtime_error("pg_parser parse: value out of range");
        } else if (result.ec == std::errc::invalid_argument) {
            throw std::runtime_error("pg_parser parse: invalid argument");
        }
        return val;
    }
};

template <>
struct pg_parser<std::string> {
    static std::string parse(const uint8_t* ptr, long size) {
        return { reinterpret_cast<const char*>(ptr), static_cast<size_t>(size) };
    }
};

template <>
struct pg_parser<bool> {
    static bool parse(const uint8_t* ptr, long size) {
        if (size != 1) {
            throw std::runtime_error("pg_parser parse: invalid bool value");
        }
        char c = static_cast<char>(*ptr);
        switch (c) {
            case 't':
                return true;
            case 'f':
                return false;
            default:
                throw std::runtime_error("pg_parser parse: invalid bool value");
        }
    }
};

constexpr long SERIALIZE_BUFFER_SIZE = 256;

template <typename T>
struct pg_serializer {
    template <typename U = T>
    static void serialize(std::vector<uint8_t>& buf, const std::enable_if_t<is_number<U>, U>& val) {
        std::array<char, SERIALIZE_BUFFER_SIZE> temp;
        std::to_chars_result result = std::to_chars(temp.data(), temp.data() + temp.size(), val);
        if (result.ec == std::errc::value_too_large) {
            throw std::runtime_error("pg_serializer serialize: value too large");
        }
        long out_size = result.ptr - temp.data();

        long index = buf.size();
        buf.resize(index + 4);
        write_u32_be(buf.data() + index, out_size);
        for (char* ptr = temp.data(); ptr != result.ptr; ++ptr) {
            buf.push_back(static_cast<uint8_t>(*ptr));
        }
    }
};

template <>
struct pg_serializer<std::string> {
    static void serialize(std::vector<uint8_t>& buf, const std::string& val) {
        long index = buf.size();
        buf.resize(index + 4);
        write_u32_be(buf.data() + index, val.size());
        for (char c : val) {
            buf.push_back(static_cast<uint8_t>(c));
        }
    }
};

template <>
struct pg_serializer<bool> {
    static void serialize(std::vector<uint8_t>& buf, const bool& val) {
        long index = buf.size();
        buf.resize(index + 4);
        write_u32_be(buf.data() + index, 1);
        if (val) {
            buf.push_back('t');
        } else {
            buf.push_back('f');
        }
    }
};

}

#endif
