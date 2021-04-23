/**
 * @file  FileUtils.h
 *
 * class FileUtils implement some useful file operation.
 *
 * @date  2021-04-23
 */

#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <string>

enum class FileType {
    UNKNOWN = 0,
    NOTEXIST,
    REGULAR,
    DIRECTORY,
    SYM_LINK,
    CHAR_DEV,
    BLOCK_DEV,
    FIFO,
    SOCKET
};

class FileUtils final {
    FileUtils() = delete;

    // Get the directory part of a path
    static std::string dirname(const char *path);

    // Get the base part of a path
    static std::string basename(const char *path);

    //Get the content of a symbol link

    // Get the canonicalized absolute pathname of a path

    // Return the size of the given file
    static size_t fileSize(const char *path);
};

#endif