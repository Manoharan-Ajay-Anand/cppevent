add_library(cppevent_net
    src/cppevent_net/socket.cpp
    src/cppevent_net/client_socket.cpp
    src/cppevent_net/server_socket.cpp
    src/cppevent_net/server.cpp
    src/cppevent_net/util.cpp
)

target_include_directories(cppevent_net PUBLIC src)

target_link_libraries(cppevent_net PUBLIC cppevent_base)
