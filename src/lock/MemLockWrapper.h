/**
 * @file  MemLockWrapper.h
 *
 * Class MemLockGuard is RAII style to easily control the MemLockCore acquire / release.
 * 
 * @date  2021-04-22
 */

#ifndef MEMLOCKWRAPPER_H
#define MEMLOCKWRAPPER_H

#include <vector>
#include "./MemLockCore.h"

template<typename KeyType>
class MemLockGuard {
public:
    MemLockGuard(const MemLockGuard&) = delete;
    MemLockGuard& operator=(const MemLockGuard&) = delete;

    // lock on a collection of keys
    MemLockGuard(MemLockCore<KeyType> *lock, const std::vector<KeyType>& keys, bool dedup = false) 
        : m_lock(lock), m_keys(keys) {
        if (dedup) {
            std::sort(m_keys.begin(), m_keys.end());
            auto last = std::unique(m_keys.begin(), m_keys.end());
            std::tie(m_iter, m_locked) = m_lock->lockBatch(m_keys.begin(), m_keys.end());
        } else {
            std::tie(m_iter, m_locked) = m_lock->lockBatch(m_keys.begin(), m_keys.end());
        }
    }

    // lock on a specific key
    MemLockGuard(MemLockCore<KeyType> *lock, const KeyType &key) 
        : MemLockGuard(lock, std::vector<KeyType>{key}){}

    MemLockGuard(MemLockGuard&& lg) noexcept 
    : m_lock(lg.m_lock), m_keys(std::move(lg.m_keys), m_locked(lg.m_locked)) {}

    MemoryLockGuard& operator=(MemoryLockGuard&& lg) noexcept {
        if (this != &lg) {
            m_lock = lg.m_lock;
            m_keys = std::move(lg.m_keys);
            m_locked = lg.m_locked;
        }
        return *this;
    }

    ~MemLockGuard() {
        if (m_locked) {
            m_lock->unlockBatch(m_keys);
        }
    }

    bool isLocked() const noexcept {
        return m_locked;
    }

    operator bool() const noexcept {
        return isLocked();
    }

    // return the first conflict key, if any
    // this has to be called when m_lock is false;
    Key conflictKey() {
        return *m_iter;
    }
private:
    MemLockCore<KeyType> *m_lock;
    std::vector<KeyType> m_keys;
    typename std::vector<Key>::iterator m_iter;
    bool m_locked = false; 
};

#endif