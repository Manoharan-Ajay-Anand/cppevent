#include <doctest/doctest.h>

#include <cppevent_http/http_request.hpp>

TEST_CASE("http request request line test") {
    cppevent::http_request req;
    
    SUBCASE("normal") {
        constexpr std::string_view input_str = "GET /api/search?fruit=apple&color=red&color=green" 
                                               " HTTP/1.1";
        CHECK(req.process_req_line(input_str));
        CHECK_EQ(req.get_method(), cppevent::HTTP_METHOD::GET);
        CHECK_EQ(req.get_version(), cppevent::HTTP_VERSION::HTTP_1_1);
        CHECK_EQ(req.get_path(), "/api/search");
        const auto& segments = req.get_path_segments();
        CHECK_EQ(segments.size(), 2);
        CHECK_EQ(segments[0], "api");
        CHECK_EQ(segments[1], "search");
        CHECK_EQ(req.get_query(), "fruit=apple&color=red&color=green");
        auto fruit_opt = req.get_query_param("fruit");
        CHECK(fruit_opt.has_value());
        CHECK_EQ(fruit_opt.value(), "apple");
        auto colors = req.get_multi_query_param("color");
        CHECK_EQ(colors.size(), 2);
        CHECK_EQ(colors[0], "red");
        CHECK_EQ(colors[1], "green");
    }

    SUBCASE("unknown method") {
        constexpr std::string_view input_str = "DUMMY /api/search?fruit=apple&color=red&color=green" 
                                               " HTTP/1.1";
        CHECK_FALSE(req.process_req_line(input_str));
    }

    SUBCASE("unknown version") {
        constexpr std::string_view input_str = "POST /api/search?fruit=apple&color=red&color=green" 
                                               " HTTP/1.2";
        CHECK_FALSE(req.process_req_line(input_str));
    }
}

TEST_CASE("http request header line test") {
    cppevent::http_request req;
    
    SUBCASE("normal") {
        constexpr std::string_view input_str = "content-type: text/html";
        CHECK(req.process_header_line(input_str));
        auto content_type_opt = req.get_header("content-type");
        CHECK(content_type_opt.has_value());
        CHECK_EQ(content_type_opt.value(), "text/html");
    }

    SUBCASE("no colon") {
        constexpr std::string_view input_str = "content-type text/html";
        CHECK_FALSE(req.process_header_line(input_str));
    }

    SUBCASE("no value") {
        constexpr std::string_view input_str = "content-type:    ";
        CHECK_FALSE(req.process_header_line(input_str));
    }
}
