#include "http_string.hpp"

#include <cstring>

cppevent::http_string::~http_string() {
    if (m_size > 0) {
        delete [] m_data;
    }
}

cppevent::http_string::http_string(const std::string_view& sv) {
    if (m_size > 0) {
        delete [] m_data;
    }
    m_size = sv.size();
    if (m_size > 0) {
        m_data = new char[m_size];
        std::memcpy(m_data, sv.data(), m_size);
    }
}

cppevent::http_string& cppevent::http_string::operator=(const std::string_view& sv) {
    if (m_size > 0) {
        delete [] m_data;
    }
    m_size = sv.size();
    if (m_size > 0) {
        m_data = new char[m_size];
        std::memcpy(m_data, sv.data(), m_size);
    }
    return *this;
}

cppevent::http_string::http_string(http_string&& other) {
    if (m_size > 0) {
        delete [] m_data;
    }
    m_data = other.m_data;
    m_size = other.m_size;
    other.m_size = 0;
}

cppevent::http_string& cppevent::http_string::operator=(http_string&& other) {
    if (m_size > 0) {
        delete [] m_data;
    }
    m_data = other.m_data;
    m_size = other.m_size;
    other.m_size = 0;
    return *this;
}

std::string_view cppevent::http_string::get_view() const {
    return { m_data, static_cast<size_t>(m_size) };
}
