#include <memory>
#include <regex>
#include <unordered_map>
#include <vector>
#include <proxygen/httpserver/RequestHandler.h>
#include <proxygen/lib/http/HTTPMethod.h>

using PathParams = std::unordered_map<std::string, std::string>;
using ReqHandlerGenerator = std::function<proxygen::RequestHandler*(PathParams &&)>;

class Route final {
public:
    Route(proxygen::HTTPMethod method, const std::string &path) : m_method(method) {
        checkPath(path);
        setPath(path);
    }

    void setNext(Route *next) {
        m_next = next;
    }

    Route* next() const {
        return m_next;
    }

    bool matches(proxygen::HTTPMethod method, const std::string &path) const;

    // Register a handler generator for the route
    void handler(ReqHandlerGenerator generator);

    proxygen::RequestHandler *generateHandler(const std::string &path) const;

private:
    static void checkPath(const std::string &path);
    void setPath(const std::string &path);
    void createPattenRegex(const std::string &path);

    static std::unique_ptr<std::regex> m_reToken;

    Route *m_next{nullptr};
    proxygen::HTTPMethod m_method;
    std::string m_path;
    std::unique_ptr<std::regex> m_pattern;
    ReqHandlerGenerator m_generator;
    std::vector<std::string> m_groups;
};

class Router final {
public:
    explicit Router(const std::string &prefix) : m_prefix(prefix), m_webService(nullptr) {}

    Router(const std::string &prefix, const WebService *webSvc) : m_prefix(prefix),
        m_webService(webSvc) {}

    proxygen::RequestHandler *dispatch(const proxygen::HTTPMessage *msg) const;

    Route &get(const std::string &path) {
        return route(proxygen::HTTPMethod::GET, path);
    }

    Route &post(const std::string &path) {
        return route(proxygen::HTTPMethod::POST, path);
    }

    Route &put(const std::string &path) {
        return route(proxygen::HTTPMethod::PUT, path);
    }

    Route &del(const std::string &path) {
        return route(proxygen::HTTPMethod::DELETE, path);
    }

    Route &route(proxygen::HTTPMethod method, const std::string &path);

private:
    void append(Route *route);

    std::string m_prefix;
    Route *m_head{nullptr};
    Route *m_tail{nullptr};
    const WebService *m_webService;
};