#ifndef CPPEVENT_FCGI_FCGI_HANDLER_HPP
#define CPPEVENT_FCGI_FCGI_HANDLER_HPP

#include "types.hpp"

#include <cppevent_base/task.hpp>
#include <cppevent_base/async_signal.hpp>

namespace cppevent {

class router;

class stream;

class output;

class fcgi_handler {
private:
    router& m_router;
public:
    fcgi_handler(router& r);
    
    awaitable_task<void> handle_request(stream& s_params, stream& s_stdin,
                                        output& o_stdout, output& o_endreq,
                                        signal_trigger close_trigger, bool close_conn);
};

}

#endif
