add_library(cppevent_fcgi
    src/cppevent_fcgi/fcgi_server.cpp
    src/cppevent_fcgi/record.cpp
    src/cppevent_fcgi/request.cpp
    src/cppevent_fcgi/stream.cpp
    src/cppevent_fcgi/stream_awaiters.cpp
    src/cppevent_fcgi/output.cpp
    src/cppevent_fcgi/fcgi_handler.cpp
    src/cppevent_fcgi/context.cpp
    src/cppevent_fcgi/router.cpp
)

target_include_directories(cppevent_fcgi PUBLIC src)

target_link_libraries(cppevent_fcgi PRIVATE cppevent_net cppevent_base)
