#ifndef CPPEVENT_FCGI_ROUTER_HPP
#define CPPEVENT_FCGI_ROUTER_HPP

#include "types.hpp"

#include <cppevent_base/task.hpp>

#include <memory>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <utility>

namespace cppevent {

class endpoint;

class context;

class stream;

class output;

class route_node {
private:
    std::unordered_map<std::string_view, std::unique_ptr<route_node>> m_paths;
    endpoint* m_get_endpoint = nullptr;
    endpoint* m_post_endpoint = nullptr;
    std::string_view m_variable;
    std::unique_ptr<route_node> m_var_node;

    endpoint*& get_endpoint_ref(REQUEST_METHOD method);
public:
    void insert(const std::vector<std::string_view>& segments, long i,
                endpoint& endpoint, REQUEST_METHOD method);
    awaitable_task<void> process(const std::vector<std::string_view>& segments, long i,
                                 context& cont, stream& s_stdin, output& o_stdout);
};

class router {
private:
    route_node m_node;
public:
    void get(std::string_view path, endpoint& endpoint);
    void post(std::string_view path, endpoint& endpoint);

    awaitable_task<void> process(context& cont, stream& s_stdin, output& o_stdout);
};

}

#endif
