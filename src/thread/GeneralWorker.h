#ifndef GENERAL_WORKER_H
#define GENERAL_WORKER_H

#include <functional>
#include <mutex>
#include <unordered_map>
#include <folly/futures/Future.h>
#include <folly/Unit.h>
#include "../base/Base.h"
#include "NamedThread.h"
#include "helpers.h"

/**
 * GenericWorker implements a event-based task executor that executes tasks asynchronously
 * in a separate thread. Like `std::thread', It takes any callable object and its optional
 * arguments as a normal, delayed or repeated task.
 *
 * GenericWorker executes tasks one after one, in the FIFO way, while tasks are non-preemptible.
 *
 * Please NOTE that, as the name indicates, this a worker thread for the general purpose,
 * but not for the performance critical situation.
 */
class GeneralWorker : public NonCopable, public NonMovable {
public:
    friend class GeneralThreadPool;

    GeneralWorker();
    ~GeneralWorker();

    /**
     * To allocate resouces and launch the internal thread which executes
     * the event loop to make this worker usable.
     *
     * Optionally, you could give the internal thread a specific name,
     * which will be shown in some system utilities like `top'.
     *
     * Once started, the worker will keep waiting for newly-added tasks indefinitely,
     * until `stop' is invoked.
     *
     * A GenericWorker MUST be `start'ed successfully before invoking
     * any other interfaces.
     */
    bool MUST_USE_RESULT start(std::string name = "");

    /**
     * Asynchronouly to notify the worker to stop handling further new tasks.
     */
    bool stop();

    /**
     * Synchronously to wait the worker to finish and exit.
     *
     * For the sake of convenience, `~GenericWorker' invokes `stop' and `wait',
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
     * @return        an instance of `folly::SemiFuture' you could wait upon
     *                for the result of `task'
     */
    template <typename F, typename ...Args>
    auto addTask(F &&task, Args &&...args)
        -> typename std::enable_if<
            !std::is_void<ReturnType<F, Args...>>::value,
            FutureType<F, Args...>
            >::type;
    template <typename F, typename...Args>
    auto addTask(F &&task, Args &&...args)
        -> typename std::enable_if<
            std::is_void<ReturnType<F, Args...>>::value,
            UnitFutureType
           >::type;

    /**
     * To add a oneshot timer task which will be executed after a while.
     * @param ms      milliseconds from now when the task get executed
     * @param task    a callable object
     * @param args    variadic arguments
     * @return        an instance of `folly::SemiFuture' you could wait upon
     *          for the result of `task'
     */
    template <typename F, typename...Args>
    auto addDelayTask(size_t ms, F &&task, Args &&...args)
        -> typename std::enable_if<
            std::is_void<ReturnType<F, Args...>>::value,
            UnitFutureType
           >::type;
    template <typename F, typename...Args>
    auto addDelayTask(size_t ms, F &&task, Args &&...args)
        -> typename std::enable_if<
            !std::is_void<ReturnType<F, Args...>>::value,
            FutureType<F, Args...>
           >::type;

    /**
     * To add a repeated timer task which will be executed in each period.
     * @param ms      interval in milliseconds
     * @param task    a callable object
     * @param args    variadic arguments
     * @return        ID of the added task, unique for this worker
     */
    template <typename F, typename...Args>
    auto addRepeatTask(size_t ms, F &&task, Args &&...args);

    /**
     * To purge or deactivate a repeated task.
     * @id      ID returned by `addRepeatTask'
     */
    void purgeTimerTask(uint64_t id);

    template <typename F, typename ...Args>
    uint64_t addTimerTask(size_t, size_t, F&&, Args&&...);
private:
    struct Timer {
        explicit Timer(std::function<void(void)> cb);
        ~Timer();
        uint64_t m_id;
        uint64_t m_delayMSec;
        uint64_t m_intervalMSec;
        std::function<void(void)> m_callback;
        struct event *m_ev{nullptr};
        GeneralWorker *m_owner{nullptr};
    };
    using TimerPtr = std::unique_ptr<Timer>;
    void purgeTimerInternal(uint64_t id);

private:
    void loop();
    void notify();
    void onNotify();
    uint64_t nextTimerId() {
        return (m_nextTimerId++ & TIMER_ID_MASK);
        return 0;
    }

private:
    static constexpr uint64_t TIMER_ID_BITS     = 6 * 8;
    static constexpr uint64_t TIMER_ID_MASK     = ((~0x0UL) >> (64 - TIMER_ID_BITS));
    std::string                                 m_name;
    std::atomic<bool>                           m_stopped{true};
    volatile uint64_t                           m_nextTimerId{0};
    struct event_base                           *m_evbase{nullptr};
    int                                         m_evfd{-1};
    struct event                                *m_notifier{nullptr};
    std::mutex                                  m_lock;
    std::vector<std::function<void()>>          m_pendingTasks;
    std::vector<TimerPtr>                       m_pendingTimers;
    std::vector<uint64_t>                       m_purgingingTimers;
    std::unordered_map<uint64_t, TimerPtr>      m_activeTimers;
    std::unique_ptr<NamedThread>                m_thread;
};

template <typename F, typename ...Args>
auto GeneralWorker::addTask(F &&f, Args &&...args)
    -> typename std::enable_if<
        !std::is_void<ReturnType<F, Args...>>::value,
        FutureType<F, Args...>
    >::type {
    auto promise = std::make_shared<folly::Promise<ReturnType<F, Args...>>>();
    auto task = std::make_shared<std::function<ReturnType<F, Args...>()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
    auto future = promise->getSemiFuture();
    {
        std::lock_guard<std::mutex> guard{m_lock};
        m_pendingTasks.emplace_back(
            [=] {
                promise->setWith(*task);
            }
        );
    }
    notify();
    return future;
}

template <typename F, typename ...Args>
auto GeneralWorker::addTask(F &&f, Args &&...args) 
    -> typename std::enable_if<
        std::is_void<ReturnType<F, Args...>>::value,
        UnitFutureType
    >::type {
    auto promise = std::make_shared<folly::Promise<folly::Unit>>();
    auto task = std::make_shared<std::function<ReturnType<F, Args...>()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
    auto future = promise->getSemiFuture();
    {
        std::lock_guard<std::mutex> guard{m_lock};
        m_pendingTasks.emplace_back(
            [=] {
                try {
                    (*task)();
                    promise->setValue(folly::unit);
                } catch (const std::exception& ex) {
                    promise->setException(ex);
                }
            }
        );
    }
    notify();
    return future;
}

template <typename F, typename ...Args>
auto GeneralWorker::addDelayTask(size_t ms, F &&f, Args &&...args) 
        -> typename std::enable_if<
            std::is_void<ReturnType<F, Args...>>::value,
            UnitFutureType
        >::type {
    auto promise = std::make_shared<folly::Promise<folly::Unit>>();
    auto task = std::make_shared<std::function<ReturnType<F, Args...> ()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
    auto future = promise->getSemiFuture();
    addTimerTask(ms, 0, [=] {
        try {
            (*task)();
            promise->setValue(folly::unit);
        } catch (const std::exception& ex) {
            promise->setException(ex);
        }
    });
    return future;
}

template <typename F, typename...Args>
auto GeneralWorker::addDelayTask(size_t ms, F &&f, Args &&...args)
        -> typename std::enable_if<
            !std::is_void<ReturnType<F, Args...>>::value,
            FutureType<F, Args...>
           >::type {
    auto promise = std::make_shared<folly::Promise<ReturnType<F, Args...>>>();
    auto task = std::make_shared<std::function<ReturnType<F, Args...> ()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    auto future = promise->getSemiFuture();
    addTimerTask(ms, 0, [=] {
        promise->setWith(*task);
    });
    return future;
}

template <typename F, typename...Args>
auto GeneralWorker::addRepeatTask(size_t ms, F &&f, Args &&...args) {
    return addTimerTask(ms,
                        ms,
                        std::forward<F>(f),
                        std::forward<Args>(args)...);
}

template <typename F, typename ...Args>
uint64_t GeneralWorker::addTimerTask(size_t delay,
                                    size_t interval,
                                    F &&f,
                                    Args &&...args) {
    auto timer = std::make_unique<Timer>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
    timer->m_delayMSec = delay;
    timer->m_intervalMSec = interval;
    timer->m_owner = this;
    auto id = 0UL;
    {
        std::lock_guard<std::mutex> guard(m_lock);
        timer->m_id = (id = nextTimerId());
        m_pendingTimers.emplace_back(std::move(timer));
    }
    notify();
    return id;
}

#endif
