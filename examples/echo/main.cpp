#include <iostream>
#include <memory>

#include <cppevent_base/task.hpp>
#include <cppevent_base/event_loop.hpp>

#include <cppevent_net/server.hpp>
#include <cppevent_net/socket.hpp>

class echo_handler : public cppevent::connection_handler {
public:
cppevent::task on_connection(std::unique_ptr<cppevent::socket> sock) {
    const std::string newline("\r\n");
    while (true) {
        std::string message;
        for (int i = co_await sock->read_c(true); i != '\n'; i = co_await sock->read_c(true)) {
            if (i != '\r') {
                message.push_back(static_cast<char>(i));
            }
        }
        if (message == "close") {
            break;
        }
        message += newline;
        co_await sock->write(message.data(), message.size());
        co_await sock->flush();
    }
}

};

int main() {
    cppevent::event_loop e_loop;
    echo_handler handler;
    cppevent::server echo_server(NULL, "2048", e_loop, handler);
    e_loop.run();
}
