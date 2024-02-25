#include <iostream>
#include <memory>

#include <cppevent_base/event_loop.hpp>
#include <cppevent_base/task.hpp>

#include <cppevent_net/client_socket.hpp>
#include <cppevent_net/socket.hpp>
#include <cppevent_net/util.hpp>

cppevent::awaitable_task<std::string> read_line(cppevent::socket& sock) {
    std::string result;
    for (int i = co_await sock.read_c(true); i != '\n'; i = co_await sock.read_c(true)) {
        if (i != '\r') {
            result.push_back(static_cast<char>(i));
        }
    }
    co_return result;
}

cppevent::awaitable_task<void> incoming_message(cppevent::socket& sock, cppevent::event_loop& loop) {
    try {
        while (true) {
            std::string message = co_await read_line(sock);
            std::cout << "remote> " << message << std::endl;
        }
    } catch (std::runtime_error error) {
        std::cout << error.what() << std::endl;
    }
    loop.stop();
}

cppevent::task start_client(const std::string& name,
                            const std::string& service,
                            cppevent::event_loop& loop) {
    cppevent::client_socket client_sock { name, service, loop };
    
    const std::string newline("\r\n");
    std::cout << "Connecting..." << std::endl;
    std::unique_ptr<cppevent::socket> net_sock = co_await client_sock.connect();
    std::cout << "Connected (Enter the messages to send to the server)" << std::endl;

    auto t = incoming_message(*net_sock, loop);

    cppevent::socket stdin_sock(STDIN_FILENO, loop);

    while (true) {
        std::string message = co_await read_line(stdin_sock);
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
    start_client(name, service, loop);
    loop.run();
    return 0;
}
