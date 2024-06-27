#include "pg_result.hpp"

bool cppevent::pg_value::is_null() const {
    return m_size < 0;
}

cppevent::result_type cppevent::pg_result::get_type() const {
    return m_type;
}

void cppevent::pg_result::set_error() {
    m_type = result_type::ERROR;
}

void cppevent::pg_result::set_suspended() {
    m_type = result_type::SUSPENDED;
}

void cppevent::pg_result::set_desc_data(std::vector<uint8_t>&& desc_data) {
    m_desc_data = std::move(desc_data);
}

void cppevent::pg_result::add_row_data(std::vector<uint8_t>&& row_data) {
    m_row_data.push_back(std::move(row_data));
}

void cppevent::pg_result::add_column(pg_column col) {
    m_columns.push_back(col);
}

void cppevent::pg_result::add_row(std::vector<pg_value>&& row) {
    m_rows.push_back(std::move(row));
}

void cppevent::pg_result::set_command_tag(std::string&& tag) {
    m_cmd_tag = std::move(tag);
    m_type = result_type::SUCCESS;
}

std::string_view cppevent::pg_result::get_command_tag() const {
    return m_cmd_tag;
}

long cppevent::pg_result::get_num_rows() const {
    return m_rows.size();
}


