#include "WebService.h"

DEFINE_int32(ws_http_port, 11000, "Port to listen on with http protocol");
DEFINE_int32(ws_http2_port, 11002, "Port to listen on with http protocol");
DEFINE_string(ws_ip, "0.0.0.0", "IP/Hostname to bind to");
DEFINE_int32(ws_threads, 4, "Number of threads for the web service");

class WebServiceHandlerFactory : public proxygen::RequestHandlerFactory {
public:
    explicit WebServiceHandlerFactory(const Router *router) : m_router(router) {}

    void onServerStart(folly::EvenetBase*) noexcept override {}

    void onServerStop() noexcept override {}

    Proxygen::RequestHandler *onRequest(proxygen::RequestHandler*,
                                        proxygen::HTTPMessage* msg) noexcept override {
        m_router.dispatch(msg);
    }

private:
    const Router *m_router;
};