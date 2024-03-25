#include "fcgi_handler.hpp"

#include "router.hpp"
#include "stream.hpp"
#include "output.hpp"
#include "context.hpp"

#include <cppevent_base/util.hpp>

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <memory>

cppevent::fcgi_handler::fcgi_handler(router& r): m_router(r) {    
}

namespace cppevent {

struct header_len {
    long m_name_len;
    long m_val_len;

    long get_total_len() const {
        return m_name_len + m_val_len;
    }
};

}

cppevent::awaitable_task<cppevent::header_len> get_lengths(cppevent::stream& s) {
    long result[2];
    uint8_t data[4];
    for (int i = 0; i < 2; ++i) {
        co_await s.read(data, 1, true);
        if ((data[0] >> 7) == 0) {
            result[i] = data[0];
            continue;
        }
        data[0] = data[0] & 0x7f;
        co_await s.read(data + 1, 3, true);
        result[i] = cppevent::read_u32_be(data);
    }
    co_return cppevent::header_len { result[0], result[1] };
}

cppevent::awaitable_task<void> get_headers(cppevent::stream& s_params,
                                           std::unordered_map<std::string_view,
                                                              std::string_view>& header_map,
                                           std::string& header_buf) {
    std::vector<cppevent::header_len> header_lens;
    while ((co_await s_params.can_read())) {
        auto header_l = co_await get_lengths(s_params);
        long total_len = header_l.get_total_len();
        if (header_l.m_val_len == 0) {
            co_await s_params.skip(total_len, true);
            continue;
        }
        co_await s_params.read(header_buf, total_len, true);
        header_lens.push_back(header_l);
    }
    const char* p = header_buf.data();
    for (auto h_len : header_lens) {
        std::string_view name = { p, static_cast<std::size_t>(h_len.m_name_len) };
        std::string_view value = { p + h_len.m_name_len, static_cast<std::size_t>(h_len.m_val_len) };
        header_map[name] = value;
        p += h_len.get_total_len();
    }
}

cppevent::awaitable_task<void> cppevent::fcgi_handler::handle_request(stream& s_params,
                                                                      stream& s_stdin,
                                                                      output& o_stdout,
                                                                      output& o_endreq,
                                                                      signal_trigger close_trigger,
                                                                      bool close_conn) {
    std::unordered_map<std::string_view, std::string_view> header_map;
    std::string header_buf;
    co_await get_headers(s_params, header_map, header_buf);
    context cont { std::move(header_map) };

    co_await m_router.process(cont, s_stdin, o_stdout);
    co_await o_stdout.end();
    
    char data[8] = {};
    co_await o_endreq.write(data, 8);

    co_await s_stdin.skip(LONG_MAX, false);
    if (close_conn) {
        close_trigger.activate();
    }
}
