#include "io_service.hpp"

#include "util.hpp"
#include "io_listener.hpp"
#include "event_callback.hpp"

#include <array>

#include <sys/eventfd.h>
#include <unistd.h>

cppevent::io_service::io_service() {
    int status = ::io_uring_queue_init(1024, &m_ring, 0);
    throw_if_error(status, "Failed to init ::io_uring: ");

    m_event_fd = ::eventfd(0, 0);
    throw_if_error(m_event_fd, "Failed to create eventfd: ");

    int result = ::io_uring_register_eventfd(&m_ring, m_event_fd);
    if (result != 0) throw_error("Failed to register eventfd: ", 0 - result);
}

cppevent::io_service::~io_service() {
    ::io_uring_queue_exit(&m_ring);

    int status = ::close(m_event_fd);
    throw_if_error(status, "Failed to close eventfd: ");
}

std::unique_ptr<cppevent::io_listener> cppevent::io_service::get_listener(int fd,
                                                                          event_bus& bus) {
    return std::make_unique<io_listener>(bus, &m_ring, fd);
}

constexpr std::array<uint8_t, 8> EVENTFD_WRITE = { 0, 0, 0, 0, 0, 0, 0, 1 };

void cppevent::io_service::interrupt() {
    int status = ::write(m_event_fd, EVENTFD_WRITE.data(), EVENTFD_WRITE.size());
    throw_if_error(status, "Failed to write eventfd: ");
}

void cppevent::io_service::add_event(e_event ev) {
    m_events.push(ev);
    interrupt();
}

std::array<uint8_t, 8> EVENTFD_READ;

std::queue<cppevent::e_event> cppevent::io_service::poll_events() {
    ::io_uring_submit(&m_ring);

    int status = ::read(m_event_fd, EVENTFD_READ.data(), EVENTFD_READ.size());
    throw_if_error(status, "Failed to read eventfd: ");

    std::queue<cppevent::e_event> result = std::move(m_events);

    ::io_uring_cqe* cqe;
    while (::io_uring_peek_cqe(&m_ring, &cqe) == 0) {
        e_event ev = { ::io_uring_cqe_get_data64(cqe), cqe->res };
        ::io_uring_cqe_seen(&m_ring, cqe);
        
        result.push(ev);
    }

    return result;
}
