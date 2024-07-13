#include <doctest/doctest.h>

#include <cppevent_http/util.hpp>

TEST_CASE("split string test") {
    SUBCASE("separator between") {
        constexpr std::string_view input_str = "apples,oranges,bananas";
        std::vector<std::string_view> result = cppevent::split_string(input_str, ',');
        CHECK_EQ(result.size(), 3);
        CHECK_EQ(result[0], "apples");
        CHECK_EQ(result[1], "oranges");
        CHECK_EQ(result[2], "bananas");
    }

    SUBCASE("separator begin") {
        constexpr std::string_view input_str = ",apples,oranges,bananas";
        std::vector<std::string_view> result = cppevent::split_string(input_str, ',');
        CHECK_EQ(result.size(), 3);
        CHECK_EQ(result[0], "apples");
        CHECK_EQ(result[1], "oranges");
        CHECK_EQ(result[2], "bananas");
    }

    SUBCASE("separator end") {
        constexpr std::string_view input_str = "apples,oranges,bananas,";
        std::vector<std::string_view> result = cppevent::split_string(input_str, ',');
        CHECK_EQ(result.size(), 3);
        CHECK_EQ(result[0], "apples");
        CHECK_EQ(result[1], "oranges");
        CHECK_EQ(result[2], "bananas");
    }

    SUBCASE("no separator") {
        constexpr std::string_view input_str = "apples,oranges,bananas";
        std::vector<std::string_view> result = cppevent::split_string(input_str, ' ');
        CHECK_EQ(result.size(), 1);
        CHECK_EQ(result[0], input_str);
    }
}

TEST_CASE("retrieve params test") {
    SUBCASE("normal") {
        constexpr std::string_view input_str = "fruit=apple&color=red&brand=ntuc";
        std::multimap<std::string_view, std::string_view> result = 
                cppevent::retrieve_params(input_str);
        CHECK_EQ(result.size(), 3);
        CHECK_EQ(result.find("fruit")->second, "apple");
        CHECK_EQ(result.find("color")->second, "red");
        CHECK_EQ(result.find("brand")->second, "ntuc");
    }

    SUBCASE("missing key") {
        constexpr std::string_view input_str = "=apple&color=red&brand=ntuc";
        std::multimap<std::string_view, std::string_view> result = 
                cppevent::retrieve_params(input_str);
        CHECK_EQ(result.size(), 2);
        CHECK_EQ(result.find("color")->second, "red");
        CHECK_EQ(result.find("brand")->second, "ntuc");
    }

    SUBCASE("missing value") {
        constexpr std::string_view input_str = "brand=&color=red&brand=ntuc";
        std::multimap<std::string_view, std::string_view> result = 
                cppevent::retrieve_params(input_str);
        CHECK_EQ(result.size(), 2);
        CHECK_EQ(result.find("color")->second, "red");
        CHECK_EQ(result.find("brand")->second, "ntuc");
    }
}

TEST_CASE("trim string test") {
    SUBCASE("normal") {
        constexpr std::string_view input_str = "  hello world   ";
        std::string_view result = cppevent::trim_string(input_str);
        CHECK_EQ(result.size(), 11);
        CHECK_EQ(result.front(), 'h');
        CHECK_EQ(result.back(), 'd');
    }

    SUBCASE("empty") {
        CHECK_EQ(cppevent::trim_string(""), "");
    }

    SUBCASE("only spaces") {
        CHECK_EQ(cppevent::trim_string("     "), "");
    }
}

TEST_CASE("lower case test") {
    std::string result = cppevent::lower_case("ABCabc");
    CHECK_EQ(result, "abcabc");
}
