#include <doctest/doctest.h>

#include <string_view>
#include <optional>

#include <cppevent_postgres/pg_params.hpp>

TEST_CASE("pg_params test") {
    cppevent::pg_params params;
    std::string_view val_i = "123";
    std::string_view val_f = "12.25";
    std::string_view val_str = "hello";

    SUBCASE("normal") {
        params.store(123, 12.25f, std::string { "hello" });
        CHECK_EQ(cppevent::read_u16_be(params.data()), 3);
        CHECK_EQ(cppevent::read_u32_be(params.data() + 2), 3);
        CHECK_EQ(std::string_view { reinterpret_cast<const char*>(params.data() + 6), 3 }, val_i);
        CHECK_EQ(cppevent::read_u32_be(params.data() + 9), 5);
        CHECK_EQ(std::string_view { reinterpret_cast<const char*>(params.data() + 13), 5 }, val_f);
        CHECK_EQ(cppevent::read_u32_be(params.data() + 18), 5);
        CHECK_EQ(std::string_view { reinterpret_cast<const char*>(params.data() + 22), 5 }, val_str);
    }

    SUBCASE("null test") {
        params.store(123, std::optional<float> {}, std::string { "hello" });
        CHECK_EQ(cppevent::read_u16_be(params.data()), 3);
        CHECK_EQ(cppevent::read_u32_be(params.data() + 2), 3);
        CHECK_EQ(std::string_view { reinterpret_cast<const char*>(params.data() + 6), 3 }, val_i);
        CHECK_EQ(static_cast<int>(cppevent::read_u32_be(params.data() + 9)), -1);
        CHECK_EQ(cppevent::read_u32_be(params.data() + 13), 5);
        CHECK_EQ(std::string_view { reinterpret_cast<const char*>(params.data() + 17), 5 }, val_str);
    }
}
