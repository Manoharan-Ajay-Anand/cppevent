#include <doctest/doctest.h>

#include <cppevent_postgres/pg_conv.hpp>

#include <vector>

TEST_CASE("pg_parser test") {
    SUBCASE("integer test") {
        constexpr char input[] = "-1234";
        signed val = cppevent::pg_parser<signed>::parse(reinterpret_cast<const uint8_t*>(input),
                                                        sizeof(input) - 1);
        CHECK_EQ(val, -1234);
    }

    SUBCASE("invalid unsigned integer test") {
        constexpr char input[] = "-1234";
        CHECK_THROWS(cppevent::pg_parser<unsigned>::parse(reinterpret_cast<const uint8_t*>(input),
                                                          sizeof(input) - 1));
    }

    SUBCASE("floating point test") {
        constexpr char input[] = "12.25";
        double d = cppevent::pg_parser<double>::parse(reinterpret_cast<const uint8_t*>(input),
                                                      sizeof(input) - 1);
        CHECK_EQ(d, 12.25);
    }


    SUBCASE("std::string test") {
        constexpr char input[] = "hello";
        std::string val = 
                cppevent::pg_parser<std::string>::parse(reinterpret_cast<const uint8_t*>(input),
                                                        sizeof(input) - 1);
        CHECK_EQ(val, "hello");
    }

    SUBCASE("bool test") {
        constexpr char input_t = 't';
        constexpr char input_f = 'f';

        bool val = 
                cppevent::pg_parser<bool>::parse(reinterpret_cast<const uint8_t*>(&input_t), 1);
        CHECK(val);

        val = 
                cppevent::pg_parser<bool>::parse(reinterpret_cast<const uint8_t*>(&input_f), 1);
        CHECK_FALSE(val);
    }

    SUBCASE("invalid bool test") {
        constexpr char input_t = 't';
        constexpr char input_e = 'h';

        CHECK_THROWS(
            cppevent::pg_parser<bool>::parse(reinterpret_cast<const uint8_t*>(&input_e), 1)
        );
        CHECK_THROWS(
            cppevent::pg_parser<bool>::parse(reinterpret_cast<const uint8_t*>(&input_t), 0)
        );
    }
}

TEST_CASE("pg_serializer test") {
    std::vector<uint8_t> buffer;

    SUBCASE("integer test") {
        cppevent::pg_serializer<signed>::serialize(buffer, -1234);
        CHECK_EQ(buffer.size(), 9);
        CHECK_EQ(cppevent::read_u32_be(buffer.data()), 5);
        CHECK_EQ(buffer[4], '-');
        CHECK_EQ(buffer[5], '1');
        CHECK_EQ(buffer[6], '2');
        CHECK_EQ(buffer[7], '3');
        CHECK_EQ(buffer[8], '4');
    }

    SUBCASE("floating point test") {
        cppevent::pg_serializer<float>::serialize(buffer, 12.34f);
        CHECK_EQ(buffer.size(), 9);
        CHECK_EQ(cppevent::read_u32_be(buffer.data()), 5);
        CHECK_EQ(buffer[4], '1');
        CHECK_EQ(buffer[5], '2');
        CHECK_EQ(buffer[6], '.');
        CHECK_EQ(buffer[7], '3');
        CHECK_EQ(buffer[8], '4');
    }

    SUBCASE("std::string test") {
        cppevent::pg_serializer<std::string>::serialize(buffer, "12345");
        CHECK_EQ(buffer.size(), 9);
        CHECK_EQ(cppevent::read_u32_be(buffer.data()), 5);
        CHECK_EQ(buffer[4], '1');
        CHECK_EQ(buffer[5], '2');
        CHECK_EQ(buffer[6], '3');
        CHECK_EQ(buffer[7], '4');
        CHECK_EQ(buffer[8], '5');
    }

    SUBCASE("bool true test") {
        cppevent::pg_serializer<bool>::serialize(buffer, true);
        CHECK_EQ(buffer.size(), 5);
        CHECK_EQ(cppevent::read_u32_be(buffer.data()), 1);
        CHECK_EQ(buffer[4], 't');
    }

    SUBCASE("bool false test") {
        cppevent::pg_serializer<bool>::serialize(buffer, false);
        CHECK_EQ(buffer.size(), 5);
        CHECK_EQ(cppevent::read_u32_be(buffer.data()), 1);
        CHECK_EQ(buffer[4], 'f');
    }
}
