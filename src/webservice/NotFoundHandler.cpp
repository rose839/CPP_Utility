#include "common.h"
#include "NotFoundHandler.h"
#include <proxygen/httpserver/ResponseBuilder.h>

void NotFoundHandler::onRequest(std::unique_ptr<proxygen::HTTPMessage>) noexcept {
    // Do nothing
}


void NotFoundHandler::onBody(std::unique_ptr<folly::IOBuf>) noexcept {
    // Do nothing, we only support GET
}


void NotFoundHandler::onEOM() noexcept {
    ResponseBuilder(downstream_)
        .status(WebServiceUtils::to(HttpStatusCode::NOT_FOUND),
                WebServiceUtils::toString(HttpStatusCode::NOT_FOUND))
        .sendWithEOM();
    return;
}


void NotFoundHandler::onUpgrade(UpgradeProtocol) noexcept {
    // Do nothing
}


void NotFoundHandler::requestComplete() noexcept {
    delete this;
}


void NotFoundHandler::onError(proxygen::ProxygenError err) noexcept {
    delete this;
}
