#include "request.hpp"

#include "record.hpp"
#include "fcgi_handler.hpp"

#include <cppevent_base/async_signal.hpp>

cppevent::request::request(int id, const signal_trigger& close_trigger, bool close_conn,
                           socket& conn, output_control& control, fcgi_handler& handler)
                                    : m_params(conn),
                                      m_stdin(conn),
                                      m_stdout(id, FCGI_STDOUT, control),
                                      m_endreq(id, FCGI_END_REQUEST, control),
                                      m_task(handler.handle_request(m_params, m_stdin, m_stdout,
                                                                    m_endreq, close_trigger,
                                                                    close_conn)) {
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
