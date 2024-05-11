#include <doctest/doctest.h>

#include <cppevent_postgres/pg_parser.hpp>

TEST_CASE("pg_parser test") {
    SUBCASE("signed test") {
        constexpr char input[] = "-1234";
        signed val = cppevent::pg_parser<signed>::parse(reinterpret_cast<const uint8_t*>(input),
                                                        sizeof(input) - 1);
        CHECK_EQ(val, -1234);
    }

    SUBCASE("invalid input signed test") {
        constexpr char input[] = "+1234";
        CHECK_THROWS(cppevent::pg_parser<signed>::parse(reinterpret_cast<const uint8_t*>(input),
                                                        sizeof(input) - 1));
    }

    SUBCASE("unsigned test") {
        constexpr char input[] = "1234";
        unsigned val = cppevent::pg_parser<unsigned>::parse(reinterpret_cast<const uint8_t*>(input),
                                                            sizeof(input) - 1);
        CHECK_EQ(val, 1234);
    }

    SUBCASE("unsigned test invalid character") {
        constexpr char input[] = "-1234";
        CHECK_THROWS(cppevent::pg_parser<unsigned>::parse(reinterpret_cast<const uint8_t*>(input),
                                                            sizeof(input) - 1));
    }

    SUBCASE("std::string test") {
        constexpr char input[] = "hello";
        std::string val = 
                cppevent::pg_parser<std::string>::parse(reinterpret_cast<const uint8_t*>(input),
                                                        sizeof(input) - 1);
        CHECK_EQ(val, "hello");
    }
}
