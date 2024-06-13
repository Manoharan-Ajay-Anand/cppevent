#include <doctest/doctest.h>

#include <cppevent_crypto/random.hpp>
#include <cppevent_crypto/encoding.hpp>

TEST_CASE("random test") {
    constexpr long RANDOM_BYTES_SIZE = 33;

    SUBCASE("base64 encoding") {
        constexpr long RANDOM_OUTPUT_EXPECTED_SIZE = RANDOM_BYTES_SIZE / 3 * 4;
        std::string random_str = 
                cppevent::generate_random_string<RANDOM_BYTES_SIZE>(cppevent::base64_encode);
        CHECK_EQ(random_str.size(), RANDOM_OUTPUT_EXPECTED_SIZE);
    }
}
