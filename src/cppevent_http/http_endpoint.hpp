#ifndef CPPEVENT_HTTP_HTTP_ENDPOINT_HPP
#define CPPEVENT_HTTP_HTTP_ENDPOINT_HPP

#include <cppevent_base/task.hpp>

namespace cppevent {

class http_request;

class http_output;

class http_body;

class http_endpoint {
public:
    virtual task<> serve(const http_request& req, http_body& body, http_output& res) = 0;
};

}

#endif
