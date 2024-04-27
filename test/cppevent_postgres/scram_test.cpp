#include <doctest/doctest.h>

#include <cppevent_crypto/crypto.hpp>
#include <cppevent_postgres/scram.hpp>

#include <cstring>

TEST_CASE("scram test") {
    cppevent::crypto crypt;
    cppevent::scram scr { crypt };

    constexpr std::string_view USERNAME = "user";
    constexpr std::string_view PASSWORD = "pencil";
    constexpr std::string_view CLIENT_NONCE = "rOprNGfwEbeRWgbNEkqO";

    constexpr std::string_view EXPECTED_CLIENT_FIRST_MSG = "n,,n=user,r=rOprNGfwEbeRWgbNEkqO";

    std::string client_first_msg = scr.generate_client_first_msg(USERNAME, CLIENT_NONCE);
    CHECK_EQ(client_first_msg, EXPECTED_CLIENT_FIRST_MSG);

    const std::string SERVER_FIRST_MSG = "r=rOprNGfwEbeRWgbNEkqO%hvYDpWUa2RaTCAfuxFIlj)hNlF$k0,s=W22ZaJ0SNY7soEsUEjb6gQ==,i=4096";
    scr.resolve_server_first_msg(SERVER_FIRST_MSG);

    constexpr std::string_view EXPECTED_CLIENT_FINAL_MSG = "c=biws,r=rOprNGfwEbeRWgbNEkqO%hvYDpWUa2RaTCAfuxFIlj)hNlF$k0,p=dHzbZapWIk4jUhN+Ute9ytag9zjfMHgsqmmiz7AndVQ=";
    std::string client_final_msg = scr.generate_client_final_msg(PASSWORD);
    CHECK_EQ(client_final_msg, EXPECTED_CLIENT_FINAL_MSG);

    std::string SERVER_FINAL_MSG = "v=6rriTRBi23WpRR/wtup+mMhUZUn/dB5nLTJRsjl95G4=";
    CHECK(scr.verify_server_final_msg(SERVER_FINAL_MSG));
}
