#include <doctest/doctest.h>

#include <vector>
#include <cstdint>
#include <cppevent_base/util.hpp>

TEST_CASE("read u16 test") {
    std::vector<uint8_t> buf = { 0xAA, 0xBB };
    uint16_t result = cppevent::read_u16_be(buf.data());
    CHECK_EQ(result, 0xAABB);
}

TEST_CASE("read u32 test") {
    std::vector<uint8_t> buf = { 0xAA, 0xBB, 0xCC, 0xDD };
    uint32_t result = cppevent::read_u32_be(buf.data());
    CHECK_EQ(result, 0xAABBCCDD);
}

TEST_CASE("write u16 test") {
    std::vector<uint8_t> buf(2);
    cppevent::write_u16_be(buf.data(), 0xAABB);
    CHECK_EQ(buf[0], 0xAA);
    CHECK_EQ(buf[1], 0xBB);
}

TEST_CASE("write u32 test") {
    std::vector<uint8_t> buf(4);
    cppevent::write_u32_be(buf.data(), 0xAABBCCDD);
    CHECK_EQ(buf[0], 0xAA);
    CHECK_EQ(buf[1], 0xBB);
    CHECK_EQ(buf[2], 0xCC);
    CHECK_EQ(buf[3], 0xDD);
}
