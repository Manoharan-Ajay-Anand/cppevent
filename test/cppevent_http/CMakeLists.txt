add_executable(cppevent_http_test
    main.cpp
    types_test.cpp
    util_test.cpp
    http_request_test.cpp
    http_router_test.cpp
)

target_link_libraries(cppevent_http_test PRIVATE cppevent uring)

add_test(NAME HttpTest COMMAND cppevent_http_test)
