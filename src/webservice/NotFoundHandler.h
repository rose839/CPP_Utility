#ifndef NOTFOUNDHANDLER_H
#define NOTFOUNDHANDLER_H

#include <proxygen/httpserver/RequestHandler.h>

class NotFoundHandler : public proxygen::RequestHandler {
public:
    NotFoundHandler() = default;

    void onRequest(std::unique_ptr<proxygen::HTTPMessage> headers)
        noexcept override;

    void onBody(std::unique_ptr<folly::IOBuf> body) noexcept override;

    void onEOM() noexcept override;

    void onUpgrade(proxygen::UpgradeProtocol proto) noexcept override;

    void requestComplete() noexcept override;

    void onError(proxygen::ProxygenError err) noexcept override;
};

#endif
