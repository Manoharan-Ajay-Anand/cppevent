add_executable(hello main.cpp)

target_link_libraries(hello PRIVATE cppevent_fcgi cppevent_net cppevent_base uring)
