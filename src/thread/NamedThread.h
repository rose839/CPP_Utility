#ifndef NAMED_THREAD_H
#define NAMED_THREAD_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <sys/types.h>
#include <sys/prctl.h>
#include <string>
#include <functional>
#include <thread>

class NamedThread final : public std::thread {
public:
    NamedThread() = default;
    NamedThread(NamedThread&&) = default;
    NamedThread &operator=(NamedThread&&) = default;

    NamedThread(const NamedThread&) = delete;
    NamedThread& operator=(const NamedThread&) = delete;

    template <typename F, typename ...Args>
    NamedThread(const std::string &name, F &&f, Args &&...args);

public:
    class Nominator {
    public:
        explicit Nominator(const std::string &name) {
            get(m_prevName);
        }

        static void set(const std::string &name) {
            ::prctl(PR_SET_NAME, name.c_str(), 0, 0, 0);
        }

        static void get(std::string &name) {
            char buf[64];
            ::prctl(PR_GET_NAME, buf, 0, 0, 0);
            name = buf;
        }

    private:
        std::string m_prevName;
    };

private:
    static void hook(const std::string &name,
                     const std::function<void()> &f) {
        if (!name.empty()) {
            Nominator::set(name);
        }
        f();
    }
};

template <typename F, typename ...Args>
NamedThread::NamedThread(const std::string &name, F &&f, Args &&...args)
    : std::thread(hook, name, std::bind(std::forward<F>(f), std::forward<Args>(args)...)) {
}

#endif