#include <iostream>

#include <cppevent_base/event_loop.hpp>

#include <cppevent_postgres/pg_database.hpp> 

cppevent::task connect_to_database(cppevent::pg_database& database) {
    cppevent::pg_connection conn = co_await database.get_connection();
    std::cout << "Connected to database" << std::endl;
}

int main() {
    cppevent::event_loop loop;
    cppevent::pg_config config { "sample", "password"  };
    cppevent::pg_database database("localhost", "5432", config, loop);
    connect_to_database(database);
    loop.run();
    return 0;
}
