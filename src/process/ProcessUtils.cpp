#include <signal.h>
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