#include <doctest/doctest.h>

#include <cppevent_crypto/encoding.hpp>

#include <cstring>

TEST_CASE("base64 test") {
    const char sample[] = "Man";

    SUBCASE("no padding") {
        int len = 3;
        std::string encoded = cppevent::base64_encode(sample, len);
        CHECK_EQ(encoded, "TWFu");
        std::vector<uint8_t> decoded = cppevent::base64_decode(encoded);
        CHECK_EQ(decoded.size(), len);
        CHECK_EQ(memcmp(decoded.data(), sample, len), 0);
    }

    SUBCASE("single padding") {
        int len = 2;
        std::string encoded = cppevent::base64_encode(sample, len);
        CHECK_EQ(encoded, "TWE=");
        std::vector<uint8_t> decoded = cppevent::base64_decode(encoded);
        CHECK_EQ(decoded.size(), len);
        CHECK_EQ(memcmp(decoded.data(), sample, len), 0);
    }

    SUBCASE("double padding") {
        int len = 1;
        std::string encoded = cppevent::base64_encode(sample, len);
        CHECK_EQ(encoded, "TQ==");
        std::vector<uint8_t> decoded = cppevent::base64_decode(encoded);
        CHECK_EQ(decoded.size(), len);
        CHECK_EQ(memcmp(decoded.data(), sample, len), 0);
    }
}
