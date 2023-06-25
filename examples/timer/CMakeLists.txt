add_executable(timer EXCLUDE_FROM_ALL main.cpp)

target_link_libraries(timer PRIVATE cppevent-base)
