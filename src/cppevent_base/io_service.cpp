#include "io_service.hpp"

#include "util.hpp"
#include "io_listener.hpp"
#include "event_callback.hpp"

cppevent::io_service::io_service() {
    int status = io_uring_queue_init(1024, &m_ring, 0);
    throw_if_error(status, "Failed to init io_uring: ");
}

cppevent::io_service::~io_service() {
    io_uring_queue_exit(&m_ring);
}

std::unique_ptr<cppevent::io_listener> cppevent::io_service::get_listener(int fd,
                                                                          event_bus& bus) {
    return std::make_unique<io_listener>(bus, &m_ring, fd);
}

cppevent::e_event cppevent::io_service::poll() {
    io_uring_cqe* cqe;
    io_uring_submit(&m_ring);
    int wait_status = io_uring_wait_cqe(&m_ring, &cqe);
    if (wait_status < 0) {
        throw_error("Failed to wait cqe: ", 0 - wait_status);
    }
    e_id notify_id = io_uring_cqe_get_data64(cqe);
    e_status status = cqe->res;
    io_uring_cqe_seen(&m_ring, cqe);
    return { notify_id, status }; 
}
