#include "GeneralThreadPool.h"
#include "GeneralWorker.h"

bool GeneralThreadPool::start(size_t nrThreads, const std::string &name = "") {
    // this pool is in use
    if (m_nrThreads != 0) {
        return false;
    }

    if (nrThreads <= 0) {
        return false;
    }

    m_nrThreads = nrThreads;
    
    auto ok = true;
    for (auto i = 0UL; ok && i < m_nrThreads; i++) {
        m_pool.emplace_back(std::make_unique<GeneralWorker>());
        ok = m_pool.back().start();
    }

    return ok;
}

bool GeneralThreadPool::stop() {
    auto ok = true;
    for (auto worker : m_pool) {
        ok = ok && worker.stop();
    }

    return ok;
}

bool GeneralThreadPool::wait() {
    auto ok = true;
    for (auto &worker : m_pool) {
        ok = ok && worker->wait();
    }
    m_nrThreads = 0;
    m_pool.clear();
    return ok;
}

void GeneralThreadPool::purgeTimerTask(uint64_t id) {
    auto idx = (id >> GeneralWorker::TIMER_ID_BITS);
    id = (id & GeneralWorker::TIMER_ID_MASK);
    m_pool[idx]->purgeTimerTask(id);
}