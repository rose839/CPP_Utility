/**
 * @file  ProcessUtils.h
 *
 * Class ProcessUtils encapsulates some process-related operations.
 *
 * @date  2021-04-21
 */

#ifndef PROCESSUTILS_H
#define PROCESSUTILS_H

#include <sys/types.h>
#include <unistd.h>
#include "status/status.h"

class ProcessUtils final {
public:
    ProcessUtils() = delete;

    // Check whether there is no existing process who has the same pid.
    static Status isPidAvailable(pid_t pid);

    // Like isPidAvailable(uint32_t), but use a file which contains a pid as input.
    // Returns Status::OK if:
    //      The pidFile does not exist.
    //      The pidFile is readable but has no valid pid.
    //      The pidFile contains a valid pid and no such process exists.
    static Status isPidAvailable(const std::string &pidFile);

    // Write pid into file, create if not exist.
    static Status makePidFile(const std::string &path, pid_t pid = 0);

    // Make current process a daemon and write the daemon's pid into pid file. 
    static Status daemonize(const std::string &path);
};

#endif