#ifndef CPPEVENT_POSTGRES_PG_RESULT_HPP
#define CPPEVENT_POSTGRES_PG_RESULT_HPP

#include <cstdint>
#include <vector>
#include <string>
#include <string_view>
#include <span>

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
    std::string_view m_name;
    format_code m_code;
};

struct pg_value {
    uint8_t* m_ptr;
    long m_size;

    bool is_null() {
        return m_size < 0;
    }
};

class pg_result {
private:
    std::string m_cmd_tag;

    std::vector<uint8_t> m_desc_data;
    std::vector<std::vector<uint8_t>> m_row_data;

    std::vector<pg_column> m_columns;
    std::vector<std::vector<pg_value>> m_rows;

    result_type m_type = result_type::PENDING;

public:
    result_type get_type() const;
    
    void set_error();

    void set_desc_data(std::vector<uint8_t>&& desc_data);
    void add_row_data(std::vector<uint8_t>&& row_data);

    void add_column(pg_column col);

    void add_row(std::vector<pg_value>&& row);

    void set_command_tag(std::string&& tag);
    std::string_view get_command_tag() const;
};

}

#endif
