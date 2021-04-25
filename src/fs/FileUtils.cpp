#include <cstring>
#include <regex>
#include "FileUtils.h"

std::string FileUtils::dirname(const char *path) {
    if (path != nullptr && *path != '\0') {
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
