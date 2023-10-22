add_library(cppevent_fcgi
    fcgi_server.cpp
    record.cpp
    request.cpp
    stream.cpp
    stream_awaiters.cpp
    output.cpp
    fcgi_handler.cpp
    context.cpp
    router.cpp
)

target_link_libraries(cppevent_fcgi PRIVATE cppevent_net cppevent_base)
