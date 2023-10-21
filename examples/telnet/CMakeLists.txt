add_executable(telnet main.cpp)

target_link_libraries(telnet PRIVATE  cppevent_net cppevent_base uring)