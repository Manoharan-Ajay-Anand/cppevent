#include "fcgi_server.hpp"

#include "record.hpp"

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

cppevent::fcgi_server::fcgi_server(const std::string& name,
                                   const std::string& service,
                                   event_loop& loop,
                                   router& router): fcgi_server(name.c_str(), 
                                                                service.c_str(),
                                                                loop,
                                                                router) {

}

cppevent::awaitable_task<void> cppevent::fcgi_server::write_res(socket& sock,
                                                                output_queue& out_queue) {
    bool ended = false;
    uint8_t padding_data[FCGI_MAX_PADDING];
    while (!ended) {
        int count = co_await out_queue.await_items();
        for (int i = 0; i < count; ++i) {
            auto& o = out_queue.front();
            if (o.m_close_conn) {
                ended = true;
                break;
            }
            uint8_t data[FCGI_HEADER_LEN];
            record& r = o.m_record;
            r.serialize(data);
            co_await sock.write(data, FCGI_HEADER_LEN);
            co_await sock.write(o.m_content, o.m_content_len);
            co_await sock.write(padding_data, r.m_padding_len);
            o.m_trigger.activate();
            out_queue.pop();
        }
        co_await sock.flush();
    }
}

cppevent::awaitable_task<void> cppevent::fcgi_server::read_req(socket& sock,
                                                               output_queue& out_queue) {
    uint8_t header_data[FCGI_HEADER_LEN];
    uint8_t padding_data[FCGI_MAX_PADDING];
    try {
        while ((co_await sock.read(header_data, FCGI_HEADER_LEN, false)) == FCGI_HEADER_LEN) {
            record r = record::parse(header_data);
            switch (r.m_type) {
                case FCGI_BEGIN_REQUEST:
                    {
                        uint8_t req_data[FCGI_BEGIN_REQ_LEN];
                        co_await sock.read(req_data, FCGI_BEGIN_REQ_LEN, true);
                        bool close_conn = (req_data[2] & FCGI_KEEP_CONN) == 0;
                        m_requests.erase(r.m_req_id);
                        m_requests.try_emplace(r.m_req_id,
                                               r.m_req_id, close_conn,
                                               std::ref(sock), std::ref(m_loop),
                                               std::ref(out_queue), std::ref(m_handler));
                    }
                    break;
                case FCGI_STDIN:
                case FCGI_PARAMS:
                    co_await m_requests.at(r.m_req_id).update(r.m_type, r.m_content_len);
                    break;
                default:
                    throw std::runtime_error("FCGI record type unrecognized");
            }
            co_await sock.read(padding_data, r.m_padding_len, true);
        }
    } catch (std::runtime_error e) {
        std::cerr << e.what() << std::endl;
    }
    out_queue.push({ true, {}, nullptr, 0, { 0, nullptr } });
}

cppevent::task cppevent::fcgi_server::on_connection(std::unique_ptr<socket> sock) {
    output_queue out_queue(m_loop);
    auto res_task = write_res(*sock, out_queue);
    auto read_task = read_req(*sock, out_queue);
    co_await res_task;
}
