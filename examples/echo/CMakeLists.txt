add_executable(echo EXCLUDE_FROM_ALL main.cpp)

target_link_libraries(echo PRIVATE cppevent-net)