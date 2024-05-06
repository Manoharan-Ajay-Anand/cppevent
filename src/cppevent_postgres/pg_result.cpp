#include "pg_result.hpp"

cppevent::result_type cppevent::pg_result::get_type() const {
    return m_type;
}

void cppevent::pg_result::set_error() {
    m_type = result_type::ERROR;
}

void cppevent::pg_result::add_row(std::vector<std::string>&& row) {
    m_rows.push_back(std::move(row));
}

void cppevent::pg_result::set_command_tag(std::string&& tag) {
    m_cmd_tag = std::move(tag);
    m_type = result_type::SUCCESS;
}

std::string_view cppevent::pg_result::get_command_tag() const {
    return m_cmd_tag;
}
