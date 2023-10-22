#include "router.hpp"

#include "endpoint.hpp"
#include "output.hpp"

#include <stdexcept>

cppevent::endpoint*& cppevent::route_node::get_endpoint_ref(REQUEST_METHOD method) {
    switch (method) {
        case REQUEST_METHOD::GET:
            return m_get_endpoint;
        case REQUEST_METHOD::POST:
            return m_post_endpoint;
    }
    throw std::runtime_error("Unsupported HTTP method");
}

void cppevent::route_node::insert(const std::vector<std::string_view>& segments, long i,
                                  endpoint& endpoint, REQUEST_METHOD method) {
    if (i == segments.size()) {
        get_endpoint_ref(method) = &endpoint;
        return;
    }
    auto segment = segments[i];
    route_node* next_node = nullptr;
    if (segment.size() > 2 && segment.front() == '{' && segment.back() == '}') {
        auto variable = segment.substr(1, segment.size() - 2);
        if (!m_var_node) {
            m_variable = variable;
            m_var_node = std::make_unique<route_node>();
        } else if (variable != m_variable) {
            throw std::runtime_error("Multiple variable definition for path segment");
        }
        next_node = m_var_node.get();
    } else {
        auto& node_ptr = m_paths[segment];
        if (!node_ptr) {
            node_ptr = std::make_unique<route_node>();
        }
        next_node = node_ptr.get();
    }
    next_node->insert(segments, i + 1, endpoint, method);
}

cppevent::awaitable_task<void> route_not_found(cppevent::output& o_stdout) {
    co_await o_stdout.write("status: 404\ncontent-length: 0\n\n");
}

cppevent::awaitable_task<void> cppevent::route_node::process(
        const std::vector<std::string_view>& segments, long i,
        context& cont, stream& s_stdin, output& o_stdout) {
    if (i == segments.size()) {
        endpoint* e = get_endpoint_ref(cont.get_req_method());
        if (e != nullptr) {
            return e->process(cont, s_stdin, o_stdout);
        }
    } else {
        auto segment = segments[i];
        auto it = m_paths.find(segment);
        if (it != m_paths.end()) {
            return it->second->process(segments, i + 1, cont, s_stdin, o_stdout);
        }
        if (m_var_node) {
            cont.set_path_segment(m_variable, segment);
            return m_var_node->process(segments, i + 1, cont, s_stdin, o_stdout);
        }
    }
    return route_not_found(o_stdout);
}

std::vector<std::string_view> split_path(std::string_view path) {
    std::vector<std::string_view> segments;
    long prev_slash = -1;
    for (long i = 0; i <= path.size(); ++i) {
        if (i < path.size() && path[i] != '/') {
            continue;
        }
        long segment_start = prev_slash + 1;
        const char * segment_p = path.data() + segment_start;
        long segment_size = i - segment_start;
        if (segment_size > 0) {
            segments.push_back({ segment_p, static_cast<std::size_t>(segment_size) });
        }
        prev_slash = i;
    }
    return segments;
}

void cppevent::router::get(std::string_view path, endpoint& endpoint) {
    std::vector<std::string_view> segments = split_path(path);
    m_node.insert(segments, 0, endpoint, REQUEST_METHOD::GET);
}

void cppevent::router::post(std::string_view path, endpoint& endpoint) {
    std::vector<std::string_view> segments = split_path(path);
    m_node.insert(segments, 0, endpoint, REQUEST_METHOD::POST);
}

cppevent::awaitable_task<void> cppevent::router::process(context& cont, stream& s_stdin,
                                                         output& o_stdout) {
    std::vector<std::string_view> segments = split_path(cont.get_path());
    return m_node.process(segments, 0, cont, s_stdin, o_stdout);
}
