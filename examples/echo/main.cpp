#include <iostream>
#include <memory>

#include <cppevent_base/task.hpp>
#include <cppevent_base/event_loop.hpp>

#include <cppevent_net/server.hpp>
#include <cppevent_net/socket.hpp>

class echo_handler : public cppevent::connection_handler {
public:
cppevent::task on_connection(std::unique_ptr<cppevent::socket> socket) {
    const std::string newline("\r\n");
    while (true) {
        std::string message = co_await socket->read_line(true);
        if (message == "close") {
            break;
        }
        message += newline;
        co_await socket->write(message.data(), message.size());
        co_await socket->flush();
    }
}

};

int main() {
    cppevent::event_loop e_loop;
    echo_handler handler;
    cppevent::server echo_server("localhost", "2048", e_loop, handler);
    e_loop.run();
}
