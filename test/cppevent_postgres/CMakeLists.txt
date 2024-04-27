add_executable(cppevent_postgres_test
    main.cpp
    scram_test.cpp
)

target_link_libraries(cppevent_postgres_test PRIVATE cppevent crypto)

add_test(NAME PostgresTest COMMAND cppevent_postgres_test)
