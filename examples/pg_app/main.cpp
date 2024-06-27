#include <iostream>
#include <string>

#include <cppevent_base/event_loop.hpp>

#include <cppevent_postgres/pg_database.hpp> 
#include <cppevent_postgres/pg_params.hpp>

std::string make_car_table = 
        "CREATE TABLE cars (id serial PRIMARY KEY, brand TEXT, model TEXT, year INTEGER);"
        "INSERT INTO cars (brand, model, year) VALUES ('MB', 'E300', 2020), ('BMW', 'M5', 2020), "
        "('TOYOTA', 'CAMRY', 2023), ('MB', 'E250', 2023), ('MB', 'S400', 2023);";

std::string select_cars = "SELECT id, brand, model, year FROM cars WHERE brand=$1 AND year=$2;";

cppevent::task connect_to_database(cppevent::pg_database& database) {
    cppevent::pg_connection conn = co_await database.get_connection();
    
    std::vector<cppevent::pg_result> results = co_await conn.query(make_car_table);
    std::cout << results[0].get_command_tag() << '\n'
              << results[1].get_command_tag() << std::endl;
    
    cppevent::pg_params params;
    params.store(std::string { "MB" }, 2023);

    co_await conn.prepare_query(select_cars, params);
    cppevent::result_type type;
    do {
        cppevent::pg_result selection = co_await conn.execute(1);
        for (int i = 0; i < selection.get_num_rows(); ++i) {
            int id, year;
            std::string brand, model;
            selection.get_row(i, id, brand, model, year);
            std::cout << id << ' ' << brand << ' ' << model << ' ' << year << std::endl; 
        }
        type = selection.get_type();
    } while (type == cppevent::result_type::SUSPENDED);

    database.release(conn);
}

int main() {
    cppevent::event_loop loop;
    cppevent::pg_config config { "sample", "password" };
    cppevent::pg_database database { "localhost", "5432", config, loop };
    connect_to_database(database);
    loop.run();
    return 0;
}
