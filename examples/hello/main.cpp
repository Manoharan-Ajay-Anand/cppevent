#include <cppevent_base/event_loop.hpp>

#include <cppevent_fcgi/fcgi_server.hpp>

int main() {
    cppevent::event_loop e_loop;
    cppevent::fcgi_server server("localhost", "9000", e_loop);
    e_loop.run();
    return 0;
}
