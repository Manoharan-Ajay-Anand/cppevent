#ifndef CPPEVENT_BASE_SPIN_LOCK_HPP
#define CPPEVENT_BASE_SPIN_LOCK_HPP

#include <atomic>

namespace cppevent {

class spin_lock_guard {
private:
    std::atomic_bool& m_unlocked;
public:
    spin_lock_guard(std::atomic_bool& unlocked);
    ~spin_lock_guard();
};

class spin_lock {
private:
    std::atomic_bool m_unlocked { true };
public:
    spin_lock_guard acquire();
};

}

#endif
