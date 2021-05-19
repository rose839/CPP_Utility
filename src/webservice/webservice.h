#ifndef WEBSERVICE_H
#define WEBSERVICE_H

#include <string>
#include <memory>
#include <proxygen/httpserver/HTTPServer.h>
#include <proxygen/httpserver/HTTPServerOptions.h>
#include <proxygen/httpserver/RequestHandlerFactory.h>
#include "../status/status.h"
#include "../thread/NamedThread.h"
#include "../base/Base.h"

class WebService final {
public:
    explicit WebService(const std::string name = "");
    ~WebService();

    MUST_USE_RESULT Router& router() {
        return *m_route;
    }

    // To start the global web server.
    // Two ports would be bound, one for HTTP, another one for HTTP2.
    // If FLAGS_ws_http_port or FLAGS_ws_h2_port is zero, an ephemeral port
    // would be assigned and set back to the gflag, respectively.
    MUST_USE_RESULT Status start();

    bool started() {
        return m_started;
    }

private:
    bool m_started{false};
    std::unique_ptr<proxygen::HTTPServer> m_server;
    std::unique_ptr<NamedThread> m_thread;
    std::unique_ptr<Router> m_router;
};

#endif