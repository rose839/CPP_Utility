#ifndef GENERAL_THREAD_POOL_H
#define GENERAL_THREAD_POOL_H

#include <string>
#include <vector>
#include <atomic>
#include <memory>
#include "helpers.h"
#include "GeneralWorker.h"

/**
 * Based on GeneralWorker, GenericThreadPool implements a thread pool that execute tasks asynchronously.
 *
 * Under the hood, GenericThreadPool distributes tasks around the internal threads in a round-robin way.
 *
 * Please NOTE that, as the name indicates, this a thread pool for the general purpose,
 * but not for the performance critical situation.
 */
class GeneralThreadPool : public NonCopable, 
                          public NonMovable {
public:
    GeneralThreadPool();
    ~GeneralThreadPool();

    /**
     * To launch the internal generic workers.
     *
     * Optionally, you could give the internal thread a specific name,
     * which will be shown in some system utilities like `top'.
     *
     * Once started, the worker will keep waiting for newly-added tasks indefinitely,
     * until `stop' is invoked.
     *
     * A GenericThreadPool MUST be `start'ed successfully before invoking
     * any other interfaces.
     *
     * @param nrThreads   number of internal threads
     * @param name        name of internal threads
     */
    bool start(size_t nrThreads, const std::string &name = "");

    /**
     * Asynchronouly to notify the workers to stop handling further new tasks.
     */
    bool stop();

    /**
     * Synchronously to wait the workers to finish and exit.
     *
     * For the sake of convenience, `~GeneralWorker' invokes `stop' and `wait',
     * but it's better to control these processes manually to make the resource
     * management more clearly.
     */
    bool wait();

    template <typename F, typename ...Args>
    using ReturnType = typename std::result_of<F(Args...)>::type;

    template <typename F, typename ...Args>
    using FutureType = folly::SemiFuture<ReturnType<F, Args...>>;

    using UnitFutureType = folly::SemiFuture<folly::Unit>;

    /**
     * To add a normal task.
     * @param task    a callable object
     * @param args    variadic arguments
     * @param return  an instance of `folly::SemiFuture' you could wait upon
     *          for the result of `task'
     */
    template <typename F, typename...Args>
    auto addTask(F&&, Args&&...)
        -> typename std::enable_if<
            !std::is_void<ReturnType<F, Args...>>::value,
            FutureType<F, Args...>
           >::type;
    template <typename F, typename ...Args>
    auto addTask(F&&, Args&&...)
        -> typename std::enable_if<
            std::is_void<ReturnType<F, Args...>>::value,
            UnitFutureType
        >::type;

    /**
     * To add a oneshot timer task which will be executed after a while.
     * @param ms      milliseconds from now when the task get executed
     * @param task    a callable object
     * @param args    variadic arguments
     * @return  an instance of `folly::SemiFuture' you could wait upon
     *          for the result of `task'
     */
    template <typename F, typename...Args>
    auto addDelayTask(size_t, F&&, Args&&...)
        -> typename std::enable_if<
            !std::is_void<ReturnType<F, Args...>>::value,
            FutureType<F, Args...>
           >::type;
    template <typename F, typename...Args>
    auto addDelayTask(size_t, F&&, Args&&...)
        -> typename std::enable_if<
            std::is_void<ReturnType<F, Args...>>::value,
            UnitFutureType
           >::type;

     /**
     * To add a repeated timer task which will be executed in each period.
     * @param ms      interval in milliseconds
     * @param task    a callable object
     * @param args    variadic arguments
     * @return  ID of the added task, unique for this worker
     */
    template <typename F, typename...Args>
    uint64_t addRepeatTask(size_t, F&&, Args&&...);

    /**
     * To purge or deactivate a repeated task.
     * @id      ID returned by `addRepeatTask'
     */
    void purgeTimerTask(uint64_t id);

private:
    size_t m_nrThreads = 0;
    std::atomic<size_t> m_nextThread{0};
    std::vector<std::unique_ptr<GeneralWorker>> m_pool;
};

template <typename F, typename ...Args>
auto GeneralThreadPool::addTask(F &&f, Args &&...args)
        -> typename std::enable_if<
            !std::is_void<ReturnType<F, Args...>>::value,
            FutureType<F, Args...>
        >::type {
    auto idx = m_nextThread++ % m_nrThreads;
    return m_pool[idx].addTask(std::forward<F>(f), std::forward<Args>(args)...);
}

template <typename F, typename...Args>
auto GeneralThreadPool::addTask(F &&f, Args &&...args)
        -> typename std::enable_if<
            std::is_void<ReturnType<F, Args...>>::value,
            UnitFutureType
           >::type {
    auto idx = m_nextThread++ % m_nrThreads;
    return m_pool[idx]->addTask(std::forward<F>(f),
                               std::forward<Args>(args)...);
}

template <typename F, typename...Args>
auto GeneralThreadPool::addDelayTask(size_t ms, F &&f, Args &&...args)
        -> typename std::enable_if<
            !std::is_void<ReturnType<F, Args...>>::value,
            FutureType<F, Args...>
           >::type {
    auto idx = m_nextThread++ % m_nrThreads;
    return m_pool[idx]->addDelayTask(ms,
                                    std::forward<F>(f),
                                    std::forward<Args>(args)...);
}


template <typename F, typename...Args>
auto GeneralThreadPool::addDelayTask(size_t ms, F &&f, Args &&...args)
        -> typename std::enable_if<
            std::is_void<ReturnType<F, Args...>>::value,
            UnitFutureType
           >::type {
    auto idx = m_nextThread++ % m_nrThreads;
    return m_pool[idx]->addDelayTask(ms,
                                    std::forward<F>(f),
                                    std::forward<Args>(args)...);
}


template <typename F, typename...Args>
uint64_t GeneralThreadPool::addRepeatTask(size_t ms, F &&f, Args &&...args) {
    auto idx = m_nextThread++ % m_nrThreads;
    auto id = m_pool[idx]->addRepeatTask(ms,
                                        std::forward<F>(f),
                                        std::forward<Args>(args)...);
    return ((idx << GeneralWorker::TIMER_ID_BITS) | id);
}

#endif
