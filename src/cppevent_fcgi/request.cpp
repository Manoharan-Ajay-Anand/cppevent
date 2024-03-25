#include "request.hpp"

#include "record.hpp"
#include "fcgi_handler.hpp"

cppevent::request::request(int id, bool close_conn, socket& conn, event_loop& loop,
                           output_control& control, signal_trigger trigger, fcgi_handler& handler)
                                    : m_params(conn, loop), m_stdin(conn, loop),
                                      m_stdout(id, FCGI_STDOUT, control, conn),
                                      m_endreq(id, FCGI_END_REQUEST, control, conn),
                                      m_task(handler.handle_request(m_params, m_stdin, m_stdout,
                                                                    m_endreq, trigger, close_conn)) {
}

cppevent::stream* cppevent::request::get_stream(int type) {
    switch (type) {
        case FCGI_PARAMS:
            return &m_params;
        case FCGI_STDIN:
            return &m_stdin;
    }
    return nullptr;
}

cppevent::stream_update_awaiter cppevent::request::update(int type, long remaining) {
    return get_stream(type)->update(remaining);
}
