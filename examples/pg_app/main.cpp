#include <iostream>
#include <string>

#include <cppevent_base/event_loop.hpp>

#include <cppevent_postgres/pg_database.hpp> 

std::string query_text = 
        "CREATE TABLE cars (id serial PRIMARY KEY, brand TEXT, model TEXT, year INTEGER);"
        "INSERT INTO cars (brand, model, year) VALUES ('MB', 'E300', 2020), ('BMW', 'M5', 2020);"
        "SELECT id, brand, model, year FROM cars;";

cppevent::task connect_to_database(cppevent::pg_database& database) {
    cppevent::pg_connection conn = co_await database.get_connection();
    
    std::vector<cppevent::pg_result> results = co_await conn.query_simple(query_text);
    std::cout << results[0].get_command_tag() << '\n'
              << results[1].get_command_tag() << std::endl;
    
    cppevent::pg_result& selection = results[2];
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
