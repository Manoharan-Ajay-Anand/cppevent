#ifndef CPPEVENT_POSTGRES_PG_RESULT_HPP
#define CPPEVENT_POSTGRES_PG_RESULT_HPP

#include <vector>
#include <string>
#include <string_view>

namespace cppevent {

enum class result_type {
    PENDING,
    SUCCESS,
    ERROR
};

enum class format_code {
    TEXT = 0,
    BINARY = 1
};

struct pg_column {
    std::string m_name;
    format_code m_code;
};

class pg_result {
private:
    std::string m_cmd_tag;
    std::vector<pg_column> m_columns;
    std::vector<std::vector<std::string>> m_rows;

    result_type m_type = result_type::PENDING;

public:
    result_type get_type() const;
    
    void set_error();

    void add_column(pg_column&& col);

    void add_row(std::vector<std::string>&& row);

    void set_command_tag(std::string&& tag);
    std::string_view get_command_tag() const;
};

}

#endif
