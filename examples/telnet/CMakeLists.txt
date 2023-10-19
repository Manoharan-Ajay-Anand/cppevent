add_executable(telnet EXCLUDE_FROM_ALL main.cpp)

target_link_libraries(telnet PRIVATE cppevent_net)