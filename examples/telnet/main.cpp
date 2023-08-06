#include <iostream>
#include <memory>

#include <cppevent_base/event_loop.hpp>
#include <cppevent_base/task.hpp>

#include <cppevent_net/client_socket.hpp>
#include <cppevent_net/socket.hpp>
#include <cppevent_net/util.hpp>

cppevent::awaitable_task<void> incoming_message(cppevent::socket& sock, cppevent::event_loop& loop) {
    try {
        while (true) {
            std::string message = co_await sock.read_line(true);
            std::cout << "remote> " << message << std::endl;
        }
    } catch (std::runtime_error error) {
        std::cout << error.what() << std::endl;
    }
    loop.stop();
}

cppevent::task start_client(cppevent::client_socket client_sock, cppevent::event_loop& loop) {
    const std::string newline("\r\n");
    std::cout << "Connecting..." << std::endl;
    std::unique_ptr<cppevent::socket> net_sock = co_await client_sock.connect();
    std::cout << "Connected (Enter the messages to send to the server)" << std::endl;

    auto t = incoming_message(*net_sock, loop);

    cppevent::set_non_blocking(STDIN_FILENO);
    cppevent::socket stdin_sock(STDIN_FILENO, loop);

    while (true) {
        std::string message = co_await stdin_sock.read_line(true);
        message += newline;
        co_await net_sock->write(message.data(), message.size());
        co_await net_sock->flush();
    }
}

int main() {
    cppevent::event_loop loop;
    std::string name, service;
    std::cout << "Enter hostname<space>port:" << std::endl;
    std::cin >> name >> service;
    start_client(cppevent::client_socket(name, service, loop), loop);
    loop.run();
    return 0;
}
