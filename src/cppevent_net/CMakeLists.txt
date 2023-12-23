add_library(cppevent_net OBJECT
    socket.cpp
    client_socket.cpp
    server_socket.cpp
    server.cpp
    util.cpp
)

target_link_libraries(cppevent_net PRIVATE cppevent_base)
