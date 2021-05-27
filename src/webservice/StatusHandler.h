#ifndef STATUSHANDLER_H
#define STATUSHANDLER_H

#include <vector>
#include "common.h"
#include <proxygen/httpserver/RequestHandler.h>

class StatusHandler : public proxygen::RequestHandler {
public:
    StatusHandler() = default;

    void onRequest(std::unique_ptr<proxygen::HTTPMessage> headers) noexcept override;

    void onBody(std::unique_ptr<folly::IOBuf> body) noexcept override;

    void onEOM() noexcept override;

    void onUpgrade(proxygen::UpgradeProtocol protocol) noexcept override;

    void requestComplete() noexcept override;

    void onError(proxygen::ProxygenError error) noexcept override;

private:
    folly::dynamic getStatus();

private:
    HttpCode m_err{HttpCode::SUCCEEDED};
    std::vector<std::string> m_statusNames;
    std::vector<std::string> m_statusAllNames{"status"};
};

#endif