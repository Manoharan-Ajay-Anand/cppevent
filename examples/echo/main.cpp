#include <iostream>
#include <memory>

#include <cppevent_base/task.hpp>
#include <cppevent_base/event_loop.hpp>

#include <cppevent_net/server_socket.hpp>
#include <cppevent_net/socket.hpp>

cppevent::task serve_request(std::unique_ptr<cppevent::socket> socket) {
    const std::string newline("\r\n");
    while (true) {
        std::string message;
        char c = 0;
        while (c != '\n') {
            co_await socket->read(&c, 1, true);
            if (c != '\r' && c != '\n') {
                message.push_back(c);
            }
        }
        if (message == "close") {
            break;
        }
        co_await socket->write(message.data(), message.size());
        co_await socket->write(newline.data(), newline.size());
        co_await socket->flush();
    }
}

cppevent::task start_server(cppevent::event_loop& loop) {
    cppevent::server_socket server_sock("localhost", "2048", loop);
    while (true) {
        serve_request(co_await server_sock.accept());
    }
}

int main() {
    cppevent::event_loop e_loop;
    start_server(e_loop);
    e_loop.run();
}
