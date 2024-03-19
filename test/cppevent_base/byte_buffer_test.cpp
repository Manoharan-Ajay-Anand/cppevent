#include <doctest/doctest.h>

#include <cppevent_base/byte_buffer.hpp>

#include <array>
#include <string>

#define STR_SIZE 8
#define ITERATIONS_COUNT 10
#define BUFFER_SIZE 80

const std::string STR("ABCDEFGH");

TEST_CASE("byte_buffer write test") {
    cppevent::byte_buffer<BUFFER_SIZE> buffer;

    SUBCASE("single write") {
        CHECK(buffer.write(STR.data(), STR_SIZE) == STR_SIZE);
        CHECK(buffer.available() == STR_SIZE);
        CHECK(buffer.capacity() == BUFFER_SIZE - STR_SIZE);
    }

    SUBCASE("multiple writes") {
        for (int i = 1; i <= ITERATIONS_COUNT; ++i) {
            CHECK(buffer.available() == (i - 1) * STR_SIZE);
            CHECK(buffer.capacity() == (ITERATIONS_COUNT - i + 1) * STR_SIZE);
            CHECK(buffer.write(STR.data(), STR_SIZE) == STR_SIZE);
        }
        CHECK(buffer.write(STR.data(), STR_SIZE) == 0);
        CHECK(buffer.available() == BUFFER_SIZE);
        CHECK(buffer.capacity() == 0);
    }

    SUBCASE("multiple writes with start offset") {
        buffer.increment_read_p(3);
        buffer.increment_write_p(3);
        for (int i = 1; i <= ITERATIONS_COUNT; ++i) {
            CHECK(buffer.available() == (i - 1) * STR_SIZE);
            CHECK(buffer.capacity() == (ITERATIONS_COUNT - i + 1) * STR_SIZE);
            CHECK(buffer.write(STR.data(), STR_SIZE) == STR_SIZE);
        }
        CHECK(buffer.write(STR.data(), STR_SIZE) == 0);
        CHECK(buffer.available() == BUFFER_SIZE);
        CHECK(buffer.capacity() == 0);
    }
}

TEST_CASE("byte_buffer read test") {
    cppevent::byte_buffer<BUFFER_SIZE> buffer;

    SUBCASE("single read") {
        buffer.write(STR.data(), STR_SIZE);
        std::array<std::byte, STR_SIZE> dest_arr;
        CHECK(buffer.read(dest_arr.data(), STR_SIZE) == STR_SIZE);
        CHECK(buffer.available() == 0);
        CHECK(buffer.capacity() == BUFFER_SIZE);
    }

    SUBCASE("multiple reads") {
        while (buffer.write(STR.data(), STR_SIZE) == STR_SIZE) {
        }
        std::array<std::byte, STR_SIZE> dest_arr;
        for (int i = 1; i <= ITERATIONS_COUNT; ++i) {
            CHECK(buffer.available() == BUFFER_SIZE - ((i - 1) * STR_SIZE));
            CHECK(buffer.capacity() == (i - 1) * STR_SIZE);
            CHECK(buffer.read(dest_arr.data(), STR_SIZE) == STR_SIZE);
        }
        CHECK(buffer.read(dest_arr.data(), STR_SIZE) == 0);
        CHECK(buffer.available() == 0);
        CHECK(buffer.capacity() == BUFFER_SIZE);
    }

    SUBCASE("multiple string reads") {
        while (buffer.write(STR.data(), STR_SIZE) == STR_SIZE) {
        }
        std::string result;
        for (int i = 1; i <= ITERATIONS_COUNT; ++i) {
            CHECK(buffer.available() == BUFFER_SIZE - ((i - 1) * STR_SIZE));
            CHECK(buffer.capacity() == (i - 1) * STR_SIZE);
            CHECK(buffer.read(result, STR_SIZE) == STR_SIZE);
        }
        CHECK(buffer.read(result, STR_SIZE) == 0);
        CHECK(buffer.available() == 0);
        CHECK(buffer.capacity() == BUFFER_SIZE);
        CHECK(result.size() == BUFFER_SIZE);
    }

    SUBCASE("multiple reads with start offset") {
        buffer.increment_read_p(3);
        buffer.increment_write_p(BUFFER_SIZE + 3);
        std::array<std::byte, STR_SIZE> dest_arr;
        for (int i = 1; i <= ITERATIONS_COUNT; ++i) {
            CHECK(buffer.available() == BUFFER_SIZE - ((i - 1) * STR_SIZE));
            CHECK(buffer.capacity() == (i - 1) * STR_SIZE);
            CHECK(buffer.read(dest_arr.data(), STR_SIZE) == STR_SIZE);
        }
        CHECK(buffer.read(dest_arr.data(), STR_SIZE) == 0);
        CHECK(buffer.available() == 0);
        CHECK(buffer.capacity() == BUFFER_SIZE);
    }
}
