#include "pg_parser.hpp"

#include <stdexcept>

int cppevent::convert_char_num(char c) {
    switch (c) {
        case '0' ... '9':
            return c - '0';
        default:
            throw std::runtime_error("convert_char_num: Invalid character");
    }
}
