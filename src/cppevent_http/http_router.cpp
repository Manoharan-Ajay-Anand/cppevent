#include "http_router.hpp"

#include <stdexcept>

cppevent::http_router::~http_router() = default;

cppevent::http_endpoint*& cppevent::http_router::get_endpoint_var(HTTP_METHOD method) {
    switch (method) {
        case HTTP_METHOD::GET:
            return m_get_endpoint;
        case HTTP_METHOD::POST:
            return m_post_endpoint;
        default:
            throw std::runtime_error("http_router get_endpoint_ptr: unsupported http method");
    }
}

struct split_path {
    std::string_view m_first;
    std::string_view m_remaining;

    bool is_root() const {
        return m_first.empty();
    }

    static split_path split(std::string_view path) {
        long start;
        for (start = 0; start < path.size() && path[start] == '/'; ++start);
        long end;
        for (end = start; end < path.size() && path[end] != '/'; ++end);
        return { path.substr(start, end - start), path.substr(end) };
    }
};

void cppevent::http_router::set_endpoint(HTTP_METHOD method,
                                         std::string_view path, http_endpoint* endpoint) {
    split_path sp = split_path::split(path);
    if (sp.is_root()) {
        http_endpoint*& e = get_endpoint_var(method);
        if (e != nullptr) {
            throw std::runtime_error("http_router set_endpoint: duplicate endpoint");
        } else {
            e = endpoint;
        }
        return;
    }
    
    std::string_view segment = sp.m_first;
    std::unique_ptr<http_router>* next;
    if (segment.starts_with(':') && segment.size() > 1) {
        std::string_view param = segment.substr(1);
        if (!m_path_param.empty() && m_path_param != param) {
            throw std::runtime_error("http_router set_endpoint: path parameter name clash");
        }
        m_path_param = param;
        next = &m_param_route;
    } else if (segment.starts_with(':')) {
        throw std::runtime_error("http_router set_endpoint: invalid path parameter name");
    } else {
        next = &(m_routes[segment]);
    }
    
    if (!static_cast<bool>(*next)) {
        *next = std::make_unique<http_router>();
    }
    (*next)->set_endpoint(method, sp.m_remaining, endpoint);
}

void cppevent::http_router::get(std::string_view path, http_endpoint* endpoint) {
    set_endpoint(HTTP_METHOD::GET, path, endpoint);
}

void cppevent::http_router::post(std::string_view path, http_endpoint* endpoint) {
    set_endpoint(HTTP_METHOD::POST, path, endpoint);
}

cppevent::http_endpoint* cppevent::http_router::match(HTTP_METHOD method,
                                                      std::string_view path,
                                                      std::unordered_map<std::string_view,
                                                                     std::string_view>& path_params) {
    split_path sp = split_path::split(path);
    if (sp.is_root()) {
        return get_endpoint_var(method);
    }

    auto it = m_routes.find(sp.m_first);
    if (it != m_routes.end()) {
        return it->second->match(method, sp.m_remaining, path_params);
    } else if (!m_path_param.empty()) {
        path_params[m_path_param] = sp.m_first;
        return m_param_route->match(method, sp.m_remaining, path_params);
    }
    return nullptr;
}