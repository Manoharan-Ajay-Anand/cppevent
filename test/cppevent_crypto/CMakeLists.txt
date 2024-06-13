add_executable(cppevent_crypto_test
    main.cpp
    encoding_test.cpp
    random_test.cpp
)

target_link_libraries(cppevent_crypto_test PRIVATE cppevent crypto)

add_test(NAME CryptoTest COMMAND cppevent_crypto_test)
