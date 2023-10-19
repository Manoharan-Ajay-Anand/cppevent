add_executable(cppevent_base_test EXCLUDE_FROM_ALL
    main.cpp
    byte_buffer_test.cpp
    awaiter_test.cpp
    event_bus_test.cpp
    util_test.cpp
)

target_link_libraries(cppevent_base_test PRIVATE
    cppevent_base
    doctest
)
