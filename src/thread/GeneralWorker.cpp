#include <sys/eventfd.h>
#include <event2/event.h>
#include <unistd.h>
#include "GeneralWorker.h"

GeneralWorker::GeneralWorker() = default;

GeneralWorker::~GeneralWorker() {
    stop();
    wait();
    if (m_notifier != nullptr) {
        event_free(m_notifier);
        m_notifier = nullptr;
    }
    if (m_evbase != nullptr) {
        event_base_free(m_evbase);
        m_evbase = nullptr;
    }
    if (m_evfd >= 0) {
        ::close(m_evfd);
    }
}

bool GeneralWorker::start(std::string name) {
    if (!m_stopped.load(std::memory_order_acquire)) {
        return false;
    }

    m_name = std::move(name);

    // create an event base
    m_evbase = event_base_new();

    // create an eventfd for async notification
    m_evfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (m_evfd == -1) {
        return false;
    }

    auto cb = [](int fd, int16_t, void *arg) {
        auto val = 0UL;
        auto len = ::read(fd, &val, sizeof(val));
        reinterpret_cast<GeneralWorker*>(arg)->onNotify();
    };
    auto events = EV_READ | EV_PERSIST;
    m_notifier = event_new(m_evbase, m_evfd, events, cb, this);
    event_add(m_notifier, nullptr);

    // launch a new thread to run the event loop
    m_thread = std::make_unique<NamedThread>(m_name, &GeneralWorker::loop, this);

    // Mark ths worker as started
    m_stopped.store(false, std::memory_order_release);

    return true;
}

bool GeneralWorker::stop() {
    if (m_stopped.load(std::memory_order_acquire)) {
        return false;
    }
    m_stopped.store(true, std::memory_order_release);
    notify();
    return true;
}

bool GeneralWorker::wait() {
    if (m_thread == nullptr) {
        return false;
    }
    m_thread->join();
    m_thread.reset();
    return true;
}

void GeneralWorker::loop() {
    event_base_dispatch(m_evbase);
}

void GeneralWorker::notify() {
    if (m_notifier == nullptr) {
        return;
    }
    auto one = 1UL;
    auto len = ::write(m_evfd, &one, sizeof(one));
}

void GeneralWorker::onNotify() {
    if (m_stopped.load(std::memory_order_acquire)) {
        event_base_loopexit(m_evbase, nullptr);
    }

    // process general tasks
    {
        decltype(m_pendingTasks) newcomings;
        {
            std::lock_guard<std::mutex> guard(m_lock);
            newcomings.swap(m_pendingTasks);
        }
        for (auto &task : newcomings) {
            task();
        }
    }

    // process timer tasks
    {
        decltype(m_pendingTimers) newcomings;
        {
            std::lock_guard<std::mutex> guard(m_lock);
            newcomings.swap(m_pendingTimers);
        }
        auto cb = [](int fd, int16_t, void *arg) {
            auto timer = reinterpret_cast<Timer*>(arg);
            auto worker = timer->m_owner;
            timer->m_callback();
            if (timer->m_intervalMSec == 0.0) {
                worker->purgeTimerInternal(timer->m_id);
            }
        };
        for (auto &timer : newcomings) {
            timer->m_ev = event_new(m_evbase, -1, EV_PERSIST, cb, timer.get());

            auto delay = timer->m_delayMSec;
            struct timeval tv;
            tv.tv_sec = delay / 1000;
            tv.tv_usec = delay % 1000 * 1000;
            evtimer_add(timer->m_ev, &tv);

            auto id = timer->m_id;
            m_activeTimers[id] = std::move(timer);
        }
    }

    // process purging timer tasks
    {
        decltype(m_purgingingTimers) newcomings;
        {
            std::lock_guard<std::mutex> guard(m_lock);
            newcomings.swap(m_purgingingTimers);
        }
        for (auto id : newcomings) {
            purgeTimerInternal(id);
        }
    }
}

GeneralWorker::Timer::Timer(std::function<void(void)> cb) {
    m_callback = std::move(cb);
}

GeneralWorker::Timer::~Timer() {
    if (m_ev != nullptr) {
        event_free(m_ev);
        m_ev = nullptr;
    }
}

void GeneralWorker::purgeTimerTask(uint64_t id) {
    {
        std::lock_guard<std::mutex> guard(m_lock);
        m_purgingingTimers.emplace_back(id);
    }
    notify();
}

void GeneralWorker::purgeTimerInternal(uint64_t id) {
    auto iter = m_activeTimers.find(id);
    if (iter != m_activeTimers.end()) {
        evtimer_del(iter->second->m_ev);
        m_activeTimers.erase(iter);
    }
}
