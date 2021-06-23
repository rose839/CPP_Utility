#include <atomic>
#include <chrono>
#include <codition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class TaskDispatch {
public:
    // singleton pattern
    static TaskDispatch& getInstance() {
        static TaskDispatch t;
        return t;
    }

    bool start() {
        auto func = [this]() {
            while (!m_interrupt.load()) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->m_taskMutex);
                    this->m_taskCv.wait(lock, [this] {
                        return this->m_interrupt.load() || !this->m_tasks.empty();
                    })
                    if (this)
                }
                task();
            }
        };
    }

private:
    TaskDispatch() {}
    std::unique_ptr<std::thread> m_thread = nullptr;
    std::atomic<bool> m_interrupt{false};
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_taskMutex;
    std::condition_variable m_taskCv;
};