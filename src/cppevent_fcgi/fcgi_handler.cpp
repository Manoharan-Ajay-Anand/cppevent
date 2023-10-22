#include "fcgi_handler.hpp"

#include "router.hpp"
#include "stream.hpp"
#include "output.hpp"
#include "context.hpp"

#include <cppevent_base/util.hpp>

#include <cstdint>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <memory>

cppevent::fcgi_handler::fcgi_handler(router& r): m_router(r) {    
}

cppevent::awaitable_task<std::pair<long, long>> get_lengths(cppevent::stream& s) {
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
    co_return { result[0], result[1] };
}

cppevent::awaitable_task<void> get_headers(cppevent::stream& s_params,
                                           std::unordered_map<std::string_view,
                                                              std::string_view>& header_map,
                                           std::vector<std::unique_ptr<char[]>>& header_buf) {
    while ((co_await s_params.can_read())) {
        auto [name_l, val_l] = co_await get_lengths(s_params);
        long total_l = name_l + val_l;
        if (val_l == 0) {
            co_await s_params.skip(total_l, true);
            continue;
        }
        char* data = new char[total_l];
        co_await s_params.read(data, total_l, true);
        std::string_view name = { data, static_cast<std::size_t>(name_l) };
        std::string_view value = { data + name_l, static_cast<std::size_t>(val_l) };
        header_map[name] = value;
        header_buf.push_back(std::unique_ptr<char[]>{ data });
    }
}

cppevent::awaitable_task<void> cppevent::fcgi_handler::handle_request(stream& s_params,
                                                                      stream& s_stdin,
                                                                      output& o_stdout,
                                                                      output& o_endreq,
                                                                      output_queue& o_queue,
                                                                      bool close_conn) {
    std::unordered_map<std::string_view, std::string_view> header_map;
    std::vector<std::unique_ptr<char[]>> header_buf;
    co_await get_headers(s_params, header_map, header_buf);
    context cont { std::move(header_map) };
    
    co_await m_router.process(cont, s_stdin, o_stdout);
    auto stdout_awaiter = o_stdout.end();
    char data[8] = {};
    auto endreq_awaiter = o_endreq.write(data, 8);
    co_await endreq_awaiter;
    co_await stdout_awaiter;
    co_await s_stdin.skip(LONG_MAX, false);
    if (close_conn) {
        o_queue.push({ true, {}, nullptr, 0, { 0, nullptr } });
    }
}
