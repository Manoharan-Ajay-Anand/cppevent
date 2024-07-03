#include "fcgi_server.hpp"

#include "record.hpp"
#include "output_control.hpp"

#include <cppevent_base/event_loop.hpp>

#include <cppevent_net/socket.hpp>

#include <iostream>
#include <unordered_map>
#include <memory>
#include <stdexcept>

cppevent::fcgi_server::fcgi_server(const char* name,
                                   const char* service,
                                   event_loop& loop,
                                   router& router): m_loop(loop),
                                                    m_handler(router),
                                                    m_server(name, service, loop, *this) {

}

cppevent::fcgi_server::fcgi_server(const char* unix_path,
                                   event_loop& loop,
                                   router& router): m_loop(loop),
                                                    m_handler(router),
                                                    m_server(unix_path, loop, *this) {
}

cppevent::awaitable_task<void> cppevent::fcgi_server::read_req(socket& sock,
                                                               output_control& control,
                                                               request_map& requests,
                                                               signal_trigger trigger) {
    uint8_t header_data[FCGI_HEADER_LEN];
    uint8_t padding_data[FCGI_MAX_PADDING];

    try {
        while (true) {
            long len_read = co_await sock.read(header_data, FCGI_HEADER_LEN, false);
            if (len_read != FCGI_HEADER_LEN) {
                break;
            }
            record r = record::parse(header_data);
            switch (r.m_type) {
                case FCGI_BEGIN_REQUEST:
                    {
                        uint8_t req_data[FCGI_BEGIN_REQ_LEN];
                        co_await sock.read(req_data, FCGI_BEGIN_REQ_LEN, true);
                        bool close_conn = (req_data[2] & FCGI_KEEP_CONN) == 0;
                        requests[r.m_req_id] = std::make_unique<request>(r.m_req_id,
                                                                         trigger,
                                                                         close_conn,
                                                                         std::ref(sock),
                                                                         std::ref(control),
                                                                         std::ref(m_handler));
                    }
                    break;
                case FCGI_STDIN:
                case FCGI_PARAMS:
                    co_await requests.at(r.m_req_id)->update(r.m_type, r.m_content_len);
                    break;
                default:
                    throw std::runtime_error("FCGI record type unrecognized");
            }
            co_await sock.read(padding_data, r.m_padding_len, true);
        }
    } catch (std::runtime_error e) {
        std::cerr << e.what() << std::endl;
    }
    trigger.activate();
}

cppevent::task cppevent::fcgi_server::on_connection(std::unique_ptr<socket> sock) {
    async_signal signal { m_loop };
    request_map requests;
    output_control control;
    
    {
        auto write_task = control.begin_res_task(*sock);
        auto read_task = read_req(*sock, control, requests, signal.get_trigger());
        co_await signal.await_signal();
    }
    
    sock->shutdown();
}
