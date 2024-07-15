#include <doctest/doctest.h>

#include <cppevent_http/types.hpp>

TEST_CASE("header map test") {
    constexpr std::string_view SAMPLE_VALUE = "value";
    constexpr std::string_view LOWER_CASE = "content-type";
    constexpr std::string_view UPPER_CASE = "CONtent-TYPE";
    cppevent::header_map headers;
    
    headers[LOWER_CASE] = SAMPLE_VALUE;

    auto l_it = headers.find(LOWER_CASE);
    CHECK(l_it != headers.end());
    CHECK_EQ(l_it->second, SAMPLE_VALUE);

    auto u_it = headers.find(UPPER_CASE);
    CHECK(u_it != headers.end());
    CHECK_EQ(u_it->second, SAMPLE_VALUE);
}
