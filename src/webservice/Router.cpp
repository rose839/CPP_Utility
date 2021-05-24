#include "Router.h"

void Route::checkPath(const std::string &path) {
    assert(!path.empty() && path[0] == '/');
}

void Route::setPath(const std::string &path) {
    auto location = path.find(':');
    if (location != std::string::npos) {
        createPattenRegex(path);
        this->m_path = path;
    } else {
        // exact path, remove the last / of path
        if (path.size() > 1 && path.back() == '/') {
            this->m_path = path.substr(0, path.length() - 1);
        } else {
            this->m_path = path.empty() ? "/" : path;
        }
    }
}

// static
std::unique_ptr<std::regex> Route::m_reToken = 
    std::make_unique<std::regex>(":([A-Za-z][A-Za-z_0-9]*)");