#include <iostream>
#include <memory>
#include <array>

#include <cppevent_base/event_loop.hpp>
#include <cppevent_base/types.hpp>
#include <cppevent_base/task.hpp>

#include <cppevent_net/client_socket.hpp>
#include <cppevent_net/socket.hpp>
#include <cppevent_net/util.hpp>

constexpr long BUFFER_SIZE = 512;

cppevent::task<std::string> read_line(cppevent::socket& sock) {
    std::string result;
    for (int i = co_await sock.read_c(true); i != '\n'; i = co_await sock.read_c(true)) {
        if (i != '\r') {
            result.push_back(static_cast<char>(i));
        }
    }
    co_return result;
}

cppevent::task<std::string> read_line(cppevent::io_listener& listener) {
    std::string result;
    std::array<char, BUFFER_SIZE> buffer;
    bool newline = false;
    do {
        cppevent::e_status size = co_await listener.on_read(buffer.data(), BUFFER_SIZE);
        for (long i = 0; i < size; ++i) {
            char c = buffer[i];
            if (c == '\n') {
                newline = true;
                break;
            } else if (c != '\r') {
                result.push_back(c);
            }
        }
    } while (!newline);
    co_return result;
}

cppevent::task<> incoming_message(cppevent::socket& sock, cppevent::event_loop& loop) {
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

cppevent::task<> start_client(const std::string& name,
                            const std::string& service,
                            cppevent::event_loop& loop) {
    cppevent::client_socket client_sock { name.c_str(), service.c_str(), loop };
    
    const std::string newline("\r\n");
    std::cout << "Connecting..." << std::endl;
    std::unique_ptr<cppevent::socket> net_sock = co_await client_sock.connect();
    std::cout << "Connected (Enter the messages to send to the server)" << std::endl;

    auto t = incoming_message(*net_sock, loop);

    std::unique_ptr<cppevent::io_listener> stdin_listener = loop.get_io_listener(STDIN_FILENO);

    while (true) {
        std::string message = co_await read_line(*stdin_listener);
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
    cppevent::task<> t = start_client(name, service, loop);
    loop.run();
    return 0;
}
