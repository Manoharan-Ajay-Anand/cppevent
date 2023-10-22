#ifndef CPPEVENT_FCGI_ENDPOINT_HPP
#define CPPEVENT_FCGI_ENDPOINT_HPP

#include "context.hpp"
#include "stream.hpp"
#include "output.hpp"

#include <cppevent_base/task.hpp>

namespace cppevent {

class endpoint {
public:
    virtual awaitable_task<void> process(const context& cont,
                                         stream& s_stdin, output& o_stdout) = 0;
};

}

#endif
