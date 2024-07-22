#include "http_server.hpp"

#include "util.hpp"
#include "http_line.hpp"
#include "http_request.hpp"
#include "http_body.hpp"

#include <cppevent_net/socket.hpp>

#include <vector>
#include <string_view>
#include <charconv>

constexpr std::string_view EMPTY_SV;
constexpr std::string_view CONTENT_LENGTH = "content-length";
constexpr std::string_view TRANSFER_ENCODING = "transfer-encoding";

cppevent::http_server::http_server(const char* name,
                                   const char* service,
                                   event_loop& loop): m_serv(name, service, loop, *this) {
}

cppevent::http_server::http_server(const char* unix_path,
                                   event_loop& loop): m_serv(unix_path, loop, *this) {
}

cppevent::task cppevent::http_server::on_connection(std::unique_ptr<socket> sock) {
    bool keep_conn = true;
    while (keep_conn) {
        http_request req;
        
        {
            http_line req_line = co_await read_http_line(*sock);
            if (!req_line.has_value() || !req.process_req_line(req_line.m_val)) break;
        }

        {
            http_line header_line;
            for (header_line = co_await read_http_line(*sock);
                 header_line.has_value(); 
                 header_line = co_await read_http_line(*sock)) {
                if (!req.process_header_line(header_line.m_val)) break;
            }
            if (!header_line.is_last_line()) break;
        }

        long content_len = 0;
        bool content_ended = true;

        std::string_view content_len_sv = req.get_header(CONTENT_LENGTH).value_or(EMPTY_SV);
        std::string_view transfer_encode_sv = req.get_header(TRANSFER_ENCODING).value_or(EMPTY_SV);

        if (!content_len_sv.empty()) {
            std::from_chars_result result = std::from_chars(content_len_sv.begin(),
                                                            content_len_sv.end(),
                                                            content_len);
            if (result.ec != std::errc {} || content_len < 0) break;
        } else if (find_case_insensitive(transfer_encode_sv, "chunked") != std::string_view::npos) {
            content_ended = false;
        }

        http_body body { content_len, content_ended, *sock };
    }
    sock->shutdown();
}
