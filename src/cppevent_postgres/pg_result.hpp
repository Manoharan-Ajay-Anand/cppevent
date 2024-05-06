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

class pg_result {
private:
    std::string m_cmd_tag;
    std::vector<std::vector<std::string>> m_rows;

    result_type m_type = result_type::PENDING;

public:
    result_type get_type() const;
    
    void set_error();

    void add_row(std::vector<std::string>&& row);

    void set_command_tag(std::string&& tag);
    std::string_view get_command_tag() const;
};

}

#endif
