/**
 * @file  MemLockCore.h
 *
 * Class MemLockCore implement an lock on a specific key, this lock
 * is based on folly::ConcurrentHashMap.
 * It only has non-blocking try-lock interface, blocking lock interface
 * has not been implemented.
 * 
 * @date  2021-04-22
 */

#ifndef MEMLOCKCORE_H
#define MEMLOCKCORE_H

#include "../status/status.h"
#include <folly/concurrency/ConcurrentHashMap.h>

template<typename KeyType>
class MemLockCore {
public:
    MemLockCore() = default;

    ~MemLockCore() = default;

    bool try_lock(const KeyType &key) {
        return m_hashMap.insert(std::make_pair(key, 0)).second;
    }

    void unlock(const KeyType &key) {
        m_hashMap.erase(key);
    }

    template<typename IterType>
    std::pair<IterType, bool> lockBatch(IterType begin, IterType end) {
        IterType curr = begin;
        bool inserted = false;
        while (curr != end) {
            std::tie(std::ignore, inserted) = m_hashMap.insert(std::make_pair(*curr, 0));
            if (!inserted) {
                unlockBatch(begin, curr);
                return std::make_pair(curr, false);
            }
            ++curr;
        }
        return std::make_pair(end, true);
    }

    template<typename CollectionType>
    auto lockBatch(CollectionType &&collection) {
        return lockBatch(collection.begin(), collection.end());
    }

    template<typename IterType>
    void unlockBatch(IterType begin, IterType end) {
        for (; begin != end; ++begin) {
            m_hashMap.erase(*begin);
        }
    }

    template<class Collection>
    auto unlockBatch(Collection&& collection) {
        return unlockBatch(collection.begin(), collection.end());
    }

    void clear() {
        hashMap_.clear();
    }

    size_t size() {
        return hashMap_.size();
    }

private:
    folly::ConcurrentHashMap<KeyType, int> m_hashMap;
};

#endif