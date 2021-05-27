#include "../status.h"
#include "StatusHandler.h"
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

WebService::WebService(const std::string& name) {
    m_router = std::make_unique<Router>(name, this);
}

WebService::~WebService() {
    m_server->stop();
    m_thread->join();
}

WebService::start() {
    if (m_started) {
        return Status::OK();
    }

    m_started = true;

    router().get("/status").handler([](web::PathParams&& params) {
        return new StatusHandler();
    });
    router().get("/").handler([](web::PathParams&& params) {
        return new StatusHandler();
    });

    std::vector<proxygen::HTTPServer::IPConfig> ips = {
        {folly::SocketAddress(FLAGS_ws_ip, FLAGS_ws_http_port, true), proxygen::HTTPServer::Protocol::HTTP},
        {folly::SocketAddress(FLAGS_ws_ip, FLAGS_ws_http2_port, true), proxygen::HTTPServer::Protocol::HTTP2},
    };

    proxygen::HTTPServerOptions options;
    options.threads = static_cast<size_t>(FLAGS_ws_threads);
    options.idleTimeout = std::chrono::milliseconds(60000);
    options.enableContentCompression = false;
    options.handlerFactories = 
        proxygen::RequestHandlerChain().addThen<WebServiceHandlerFactory>(m_router.get()).build();
    options.h2cEnabled = true;

    m_server = std::make_unique<HTTPServer>(std::move(options));
    m_server.bind(ips);

    std::conditional_variable cv;
    std::mutex mut;
    auto status = Status::OK();
    bool serverStartedDone = false;

    m_thread = std::make_unique<NamedThread>(
        "webservice-listener",
        [&]() {
            m_server->start(
                [&]() {
                    auto addresses = m_server->address();
                    if (FLags_ws_http_port == 0) {
                        FLags_ws_http_port = adddresses[0].address.getPort();
                    }
                    if (FLags_ws_http2_port == 0) {
                        FLags_ws_http2_port = addresses[1].address.getPort();
                    }
                    {
                        std::lock_guard<std::mutex> g(mut);
                        serverStartedDone = true;
                    }
                    cv.notify_all();
                },
                [&] (std::exception_ptr eptr) {
                    CHECK(eptr);
                    try {
                        std::rethrow_exception(eptr);
                    } catch (const std::exception &e) {
                        status = Status::Error("%s", e.what());
                    }
                    {
                        std::lock_guard<std::mutex> g(mut);
                        serverStartedDone = true;
                    }
                    cv.notify_all();
                });
        });

    {
        std::unique_lock<std::mutex> lck(mut);
        cv.wait(lck, [&]() {
            return serverStartedDone;
        });

        if (!status.ok()) {
            return status;
        }
    }
    return status;        
}