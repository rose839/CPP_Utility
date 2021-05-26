#incldue <cstdlib>
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


void Router::createPattenRegex(const std::string& path) {
    int pos = 0;
    std::stringstream ss;

    for (std::sregex_iterator next(path.begin(), path.end(), *m_reToken), end; next != end; next++) {
        std::string std = next->str();
        if (!str.empty() && str.front() == ':') {
            str = str.substr(1);
        }

        for (auto& group : m_groups) {
            if (str == group) {
                abort();
            }
        }
        m_groups.emplace_back(std::move(str));
        ss << path.substr(pos, next->position() - pos) << "([^/]+)";
        pos = next->position() + next->str().length();
    }
    ss << path.substr(pos);
    this->m_pattern = std::make_unique<std::regex>(ss.str());
}

bool Route::matches(proxygen::HTTPMethod method, const std::string& path) const {
    if (m_method != method) {
        return false;
    }

    std::string p = path;
    if (p.empty()) {
        p = "/";
    } else if (p.size() > 1 && p.back() == '/') {
        p.pop_back();
    }

    if (m_pattern) {
        return std::regex_search(p, *m_pattern);
    } else {
        return m_path == p;
    }
}

void Route::handler(ReqHandlerGenerator generator) {
    m_generator = generator;
}

proxygen::RequestHandler* Route::generateHandler(const string& path) {
    if (!m_pattern) {
        return m_generator({});
    }

    std::smatch m;
    std::regex_search(path, m, *m_pattern);
    PathParams params;
    if (m_groups.size() != m.size()-1) {
        abort();
    }

    for (std::size_t i = 0; i < m_groups.size(); i++) {
        params.emplace(m_groups[i], m[i+1]);
    }
    return m_generator(std::move(params));
}

proxygen::RequestHandler* Router::dispatch(const proxygen::HTTPMessage *msg) {
    for (Route *r = m_head; r != nullptr; r = r->next()) {
        if (r->matches(msg->getMethod().value(), msg->getPath())) {
            return r->generateHandler(msg->getPath());
        }
    }
    return new NotFoundHandler();
}

Route& Router::route(proxygen::HTTPMethod method, const std::string& path) {
    if (m_webService && !m_webService->started()) {
        abort();
    }

    Route *next = nullptr;
    if (!m_prefix.empty()) {
        next = new Route(method, "/" + m_prefix + (path.empty() ? "/" : path));
    } else {
        next = new Route(method, path.empty() ? "/" : path);
    }
    append(next);
    return *next;
}

void Router::append(Route *route) {
    if (m_tail != nullptr) {
        m_tail->setNext(route);
        m_tail = route;
    } else {
        m_head = m_tail = route;
    }
}