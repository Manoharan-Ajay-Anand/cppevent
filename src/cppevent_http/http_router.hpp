#ifndef CPPEVENT_HTTP_HTTP_ROUTER_HPP
#define CPPEVENT_HTTP_HTTP_ROUTER_HPP

#include "types.hpp"

#include <string_view>
#include <unordered_map>
#include <memory>

namespace cppevent {

class http_endpoint;

class http_router {
private:
    http_endpoint* m_get_endpoint = nullptr;
    http_endpoint* m_post_endpoint = nullptr;

    std::unordered_map<std::string_view, std::unique_ptr<http_router>> m_routes;

    std::string_view m_path_param;
    std::unique_ptr<http_router> m_param_route;

    http_endpoint*& get_endpoint_var(HTTP_METHOD method);

    void set_endpoint(HTTP_METHOD method, std::string_view path, http_endpoint* endpoint);

public:
    ~http_router();

    void get(std::string_view path, http_endpoint* endpoint);

    void post(std::string_view path, http_endpoint* endpoint);

    http_endpoint* match(HTTP_METHOD method,
                         std::string_view path,
                         std::unordered_map<std::string_view, std::string_view>& path_params);
};

}

#endif
