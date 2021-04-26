#include <cstring>
#include <regex>
#include "FileUtils.h"

std::string FileUtils::dirname(const char *path) {
    if (path == nullptr || *path == '\0') {
        return "";
    }

    if (!::strcmp(path, "/")) {
        return "/";
    }

    static const std::regex pattern("(.*)/([^/]+)/?");
    std::cmatch result;
    if (std::regex_match(path, result, pattern)) {
        if (result[1].first == result[1].second) {    // "/path" or "/path/"
            return "/";
        }
        return result[1].str();     // "/path/to", "path/to", or "path/to/"
    }
    return ".";
}

std::string FileUtils::basename(const char *path) {
    if (path == nullptr || *path == '\0') {
        return "";
    }

    if (!::strcmp(path, "/")) {
        return "";
    }

    static const std::regex pattern("(/*([^/]+/+)*)([^/]+)/?");
    std::cmatch result;
    std::regex_match(path, result, pattern);
    return result[3].str();
}

size_t FileUtils::fileSize(const char *path) {
    struct stat st;
    if (lstat(path, &st)) {
        return 0;
    }
    return st.st_size;
}

FileType FileUtils::fileType(const char *path) {
    struct stat st;
    if (lstat(path, &st)) {
        if (errno == ENOENT) {
            return FileType::NOTEXIST;
        }
        return FileType::UNKNOWN;
    }

    if (S_ISREG(st.st_mode)) {
        return FileType::REGULAR;
    } else if (S_ISDIR(st.st_mode)) {
        return FileType::DIRECTORY;
    } else if (S_ISLNK(st.st_mode)) {
        return FileType::SYM_LINK;
    } else if (S_ISCHR(st.st_mode)) {
        return FileType::CHAR_DEV;
    } else if (S_ISBLK(st.st_mode)) {
        return FileType::BLOCK_DEV;
    } else if (S_ISFIFO(st.st_mode)) {
        return FileType::FIFO;
    } else if (S_ISSOCK(st.st_mode)) {
        return FileType::SOCKET;
    }

    return FileType::UNKNOWN;
}
