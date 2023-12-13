#include <cppevent_base/event_loop.hpp>

#include <cppevent_fcgi/fcgi_server.hpp>
#include <cppevent_fcgi/endpoint.hpp>
#include <cppevent_fcgi/router.hpp>

class hello_endpoint : public cppevent::endpoint {
public:
    cppevent::awaitable_task<void> process(const cppevent::context& cont,
                                           cppevent::stream& s_stdin, cppevent::output& o_stdout) {
        co_await o_stdout.write("content-length: 5\ncontent-type: text/plain\n\nhello");
    }
};

int main() {
    hello_endpoint hello;
    cppevent::router routes;
    routes.get("/api/hello", hello);
    cppevent::event_loop e_loop;
    cppevent::fcgi_server server(NULL, "9000", e_loop, routes);
    e_loop.run();
    return 0;
}
