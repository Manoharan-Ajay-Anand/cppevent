add_executable(hello EXCLUDE_FROM_ALL main.cpp)

target_link_libraries(hello PRIVATE cppevent_fcgi)
