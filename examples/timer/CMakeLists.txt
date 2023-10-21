add_executable(timer main.cpp)

target_link_libraries(timer PRIVATE cppevent_base uring)
