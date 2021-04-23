#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <regex>
#include "ProcessUtils.h"

Status ProcessUtils::isPidAvailable(pid_t pid) {
    if (pid == getpid()) {
        return Status::OK();
    }

    constexpr auto SIG_OK = 0;
    if (::kill(pid, SIG_OK) == 0) {
        return Status::Error("Process `%d' already existed", pid);
    }
    if (errno == EPERM) {
        return Status::Error("Process `%d' already existed but denied to access", pid);
    }
    if (errno != ESRCH) {
        return Status::Error("Uknown error: `%s'", ::strerror(errno));
    }
    return Status::OK();
}

/*
Status ProcessUtils::isPidAvailable(const std::string &pidFile) {
    if (::access(pidFile.c_str(), R_OK) == -1) {
        if (errno == ENOENT) {
            return Status::OK();
        } else {
            return Status::Error("%s: %s", pidFile.c_str(), ::strerror(errno));
        }
    }

    static const std::regex pattern("([0-9])+");
}

Status ProcessUtils::daemonize(const std::string &pidFile) {
    auto pid = ::fork();
    if (pid < 0) {
        return Status::Error("fork: %s", ::strerror(errno));
    }
    if (pid > 0) {
        ::exit(0);
    }

    // Make the child process as the session leader and detach with the controlling terminal
    ::setsid();

    // set "/dev/null" as standard input
    auto fd = ::open("/dev/null", O_RDWR);
    ::dup2(fd, 0);
    ::close(fd);

    return makePidFile(pidFile);
}

*/