#include <iostream>

#include <cppevent_base/event_loop.hpp>

#include <cppevent_postgres/pg_database.hpp> 

cppevent::task connect_to_database(cppevent::pg_database& database) {
    cppevent::pg_connection conn = co_await database.get_connection();
    
    co_await conn.query("CREATE TABLE cars (id serial PRIMARY KEY, brand TEXT, model TEXT, year INTEGER);");
    cppevent::pg_result creation = co_await conn.get_result();
    std::cout << creation.get_command_tag() << std::endl;
    
    co_await conn.query("INSERT INTO cars (brand, model, year) VALUES ('MB', 'E300', 2020), ('BMW', 'M5', 2020);");
    cppevent::pg_result insertion = co_await conn.get_result();
    std::cout << insertion.get_command_tag() << std::endl;

    co_await conn.query("SELECT id, brand, model, year FROM cars;");
    cppevent::pg_result selection = co_await conn.get_result();
    for (int i = 0; i < selection.get_num_rows(); ++i) {
        int id, year;
        std::string brand, model;
        selection.get_row(i, id, brand, model, year);
        std::cout << id << ' ' << brand << ' ' << model << ' ' << year << std::endl; 
    }
    std::cout << selection.get_command_tag() << std::endl;
}

int main() {
    cppevent::event_loop loop;
    cppevent::pg_config config { "sample", "password" };
    cppevent::pg_database database { "localhost", "5432", config, loop };
    connect_to_database(database);
    loop.run();
    return 0;
}
