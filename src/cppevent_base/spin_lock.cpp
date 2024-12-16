#include "spin_lock.hpp"

cppevent::spin_lock_guard::spin_lock_guard(std::atomic_bool& unlocked): m_unlocked(unlocked) {
}

cppevent::spin_lock_guard::~spin_lock_guard() {
    m_unlocked.store(true, std::memory_order_release);
}

cppevent::spin_lock_guard cppevent::spin_lock::acquire() {
    while (!m_unlocked.exchange(false, std::memory_order_acquire));
    return spin_lock_guard { m_unlocked };
}
