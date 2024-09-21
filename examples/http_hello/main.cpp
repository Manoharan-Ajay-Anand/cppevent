#include <cppevent_base/event_loop.hpp>

#include <cppevent_http/http_server.hpp>
#include <cppevent_http/http_output.hpp>
#include <cppevent_http/http_endpoint.hpp>
#include <cppevent_http/http_router.hpp>

constexpr std::string_view RES_MESSAGE = "Hello, from http";

class hello_endpoint : public cppevent::http_endpoint {
public:
    cppevent::task<> serve(const cppevent::http_request&,
                           cppevent::http_body&, cppevent::http_output& res) {
        res.set_status(cppevent::HTTP_STATUS::OK);
        res.set_content_length(RES_MESSAGE.size());
        res.set_header("content-type", "text/plain");
        co_await res.write(RES_MESSAGE);
    }
};

int main() {
    hello_endpoint hello;
    cppevent::http_router routes;
    routes.get("/api/hello", &hello);
    cppevent::event_loop e_loop;
    cppevent::http_server server(NULL, "8080", routes, e_loop);
    e_loop.run();
    return 0;
}
