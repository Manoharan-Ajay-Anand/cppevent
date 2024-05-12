#include <doctest/doctest.h>

#include <cppevent_postgres/pg_result.hpp>

TEST_CASE("pg_result test") {
    char ID_VAL_1[] = "1";
    char ID_VAL_2[] = "2";
    char ROLE_VAL[] = "member";

    cppevent::pg_result result;
    
    cppevent::pg_value p_id_1 { reinterpret_cast<uint8_t*>(ID_VAL_1), sizeof(ID_VAL_1) - 1 };
    cppevent::pg_value p_id_2 { reinterpret_cast<uint8_t*>(ID_VAL_2), sizeof(ID_VAL_2) - 1 };
    cppevent::pg_value p_role { reinterpret_cast<uint8_t*>(ROLE_VAL), sizeof(ROLE_VAL) - 1 };
    cppevent::pg_value p_null_role { reinterpret_cast<uint8_t*>(ROLE_VAL), -1 };

    SUBCASE("normal test") {
        result.add_row({ p_id_1, p_role });
        int id;
        std::string role;
        result.get_row(0, id, role);
        CHECK_EQ(id, 1);
        CHECK_EQ(role, "member");
    }

    SUBCASE("null test") {
        result.add_row({ p_id_1, p_null_role });
        result.add_row({ p_id_2, p_role });

        int id;
        std::optional<std::string> role;
        
        result.get_row(0, id, role);
        CHECK_EQ(id, 1);
        CHECK_FALSE(role.has_value());

        result.get_row(1, id, role);
        CHECK_EQ(id, 2);
        CHECK(role.has_value());
        CHECK_EQ(role.value(), "member");
    }

    SUBCASE("assign null to non-optional test") {
        result.add_row({ p_id_1, p_null_role });
        int id;
        std::string role;
        CHECK_THROWS(result.get_row(0, id, role));
    }
}
