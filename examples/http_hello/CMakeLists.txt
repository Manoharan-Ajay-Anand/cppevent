add_executable(http_hello main.cpp)

target_link_libraries(http_hello PRIVATE cppevent uring)
