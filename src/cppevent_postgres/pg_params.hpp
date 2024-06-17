#ifndef CPPEVENT_POSTGRES_PG_PARAMS_HPP
#define CPPEVENT_POSTGRES_PG_PARAMS_HPP

#include "pg_conv.hpp"

#include <cppevent_base/util.hpp>

#include <optional>
#include <vector>
#include <cstdint>

namespace cppevent {

class pg_params {
private:
    long m_count = 0;
    std::vector<uint8_t> m_buffer;

public:
    long count() const;

    long size() const;

    const uint8_t* data() const;

    template <typename T>
    void store(const T& val) {
        ++m_count;
        pg_serializer<T>::serialize(m_buffer, val);
    }

    template <typename T>
    void store(const std::optional<T>& val_opt) {
        if (!val_opt.has_value()) {
            ++m_count;
            long index = m_buffer.size();
            m_buffer.resize(index + 4);
            write_u32_be(m_buffer.data() + index, static_cast<uint32_t>(-1));
        } else {
            store(val_opt.value());
        }
    }

    template <typename T, typename... Args>
    void store(const T& val, const Args&... args) {
        store(val);
        store(args...);
    }
};

}

#endif
