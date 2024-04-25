#ifndef CPPEVENT_POSTGRES_TYPES_HPP
#define CPPEVENT_POSTGRES_TYPES_HPP

#include <string>

namespace cppevent {

struct pg_config {
    std::string m_user;
    std::string m_password;
    std::string m_database;
    long m_max_connections = 1;
};


}

#endif