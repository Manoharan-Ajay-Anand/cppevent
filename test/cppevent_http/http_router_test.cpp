#include <doctest/doctest.h>

#include <cppevent_http/http_endpoint.hpp>
#include <cppevent_http/http_output.hpp>
#include <cppevent_http/http_body.hpp>
#include <cppevent_http/http_request.hpp>
#include <cppevent_http/http_router.hpp>

class sample_endpoint : public cppevent::http_endpoint {
public:
    cppevent::task<> serve(const cppevent::http_request& req,
                           cppevent::http_body& body, cppevent::http_output& res) {
        co_await body.skip(LONG_LONG_MAX);
    }
};

TEST_CASE("http router") {
    sample_endpoint get_endpoint;
    sample_endpoint post_endpoint;
    cppevent::http_router router;

    std::unordered_map<std::string_view, std::string_view> path_params;

    SUBCASE("normal") {
        constexpr std::string_view GET_PATH = "/sample/get";
        constexpr std::string_view POST_PATH = "/sample/post";
        constexpr std::string_view NO_PATH = "/sample";
        router.get(GET_PATH, &get_endpoint);
        router.post(POST_PATH, &post_endpoint);

        CHECK_EQ(router.match(cppevent::HTTP_METHOD::GET, GET_PATH, path_params), &get_endpoint);
        CHECK_EQ(router.match(cppevent::HTTP_METHOD::POST, POST_PATH, path_params), &post_endpoint);
        
        CHECK_EQ(router.match(cppevent::HTTP_METHOD::POST, GET_PATH, path_params), nullptr);
        CHECK_EQ(router.match(cppevent::HTTP_METHOD::GET, POST_PATH, path_params), nullptr);
        CHECK_EQ(router.match(cppevent::HTTP_METHOD::POST, NO_PATH, path_params), nullptr);
        CHECK_EQ(router.match(cppevent::HTTP_METHOD::GET, NO_PATH, path_params), nullptr);
    }

    SUBCASE("path param") {
        constexpr std::string_view PARAM_PATH = "/dept/:dept_name/staff/:staff_id";
        constexpr std::string_view ACTUAL_PATH = "/dept/operations/staff/e456";
        router.get(PARAM_PATH, &get_endpoint);

        CHECK_EQ(router.match(cppevent::HTTP_METHOD::GET, ACTUAL_PATH, path_params), &get_endpoint);
        CHECK_EQ(path_params.size(), 2);
        CHECK_EQ(path_params.at("dept_name"), "operations");
        CHECK_EQ(path_params.at("staff_id"), "e456");
    }
}
