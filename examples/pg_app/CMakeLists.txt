add_executable(pg_app main.cpp)

target_link_libraries(pg_app PRIVATE cppevent uring crypto)
