#include <doctest/doctest.h>

#include <cppevent_postgres/pg_result.hpp>

TEST_CASE("pg_result test") {
    char ID_VAL[] = "1";
    char ROLE_VAL[] = "member";

    cppevent::pg_result result;
    
    cppevent::pg_value p_id { reinterpret_cast<uint8_t*>(ID_VAL), sizeof(ID_VAL) - 1 };
    cppevent::pg_value p_role { reinterpret_cast<uint8_t*>(ROLE_VAL), sizeof(ROLE_VAL) - 1 };
    cppevent::pg_value p_null_role { reinterpret_cast<uint8_t*>(ROLE_VAL), -1 };

    SUBCASE("normal test") {
        result.add_row({ p_id, p_role });
        int id;
        std::string role;
        result.get_row(0, id, role);
        CHECK_EQ(id, 1);
        CHECK_EQ(role, "member");
    }

    SUBCASE("null test") {
        result.add_row({ p_id, p_null_role });
        result.add_row({ p_id, p_role });

        int id;
        std::optional<std::string> role;
        
        result.get_row(0, id, role);
        CHECK_EQ(id, 1);
        CHECK_FALSE(role.has_value());

        result.get_row(1, id, role);
        CHECK_EQ(id, 1);
        CHECK(role.has_value());
        CHECK_EQ(role.value(), "member");
    }

    SUBCASE("assign null to non-optional test") {
        result.add_row({ p_id, p_null_role });
        int id;
        std::string role;
        CHECK_THROWS(result.get_row(0, id, role));
    }
}
