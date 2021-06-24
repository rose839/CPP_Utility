#include <atomic>
#include <chrono>
#include <condition_variable>
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
            while (!m_stopped.load()) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->m_taskMutex);
                    this->m_taskCv.wait(lock, [this] {
                        return this->m_stopped.load() || !this->m_tasks.empty();
                    });
                    if (this->m_stopped.load()) {
                        break;
                    }
                    task = std::move(this->m_tasks.front());
                    this->m_tasks.pop();
                }
                task();
            }
        };

        m_thread = std::make_unique<std::thread>(func);
        return true;
    }

    bool stop() {
        m_stopped.store(true);
        if (m_thread && m_thread->joinable()) {
            m_thread->join();
        }
        return true;
    }

    template<typename F, typename... Args>
    auto run(F &&f, Args &&... args) -> std::shared_ptr<std::future<std::result_of_t<F(Args...)>>> {
        using returnType = std::result_of_t<F(Args...)>;

        auto task = std::make_shared<std::packaged_task<returnType()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        std::future<returnType> ret = task->get_future();
        {
            std::lock_guard<std::mutex> lock(this->m_taskMutex);
            this->m_tasks.emplace([task](){ (*task)(); });
        }
        this->m_taskCv.notify_all();
        
        return std::make_shared<std::future<std::result_of_t<F(Args...)>>>(std::move(ret));
    }

private:
    TaskDispatch() {}
    std::unique_ptr<std::thread> m_thread = nullptr;
    std::atomic<bool> m_stopped{false};
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_taskMutex;
    std::condition_variable m_taskCv;
};
