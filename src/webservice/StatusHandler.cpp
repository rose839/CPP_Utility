#include <proxygen/httpserver/RequestHandler.h>
#include <proxygen/lib/http/ProxygenErrorEnum.h>
#include <proxygen/httpserver/ResponseBuilder.h>
#include "StatusHandler.h"

void StatusHandler::onRequest(std::unique_ptr<HTTPMessage> headers) {
    if (headers->getMethod().value() != proxygen::HTTPMethod::GET) {
        m_err = HttpCode::E_UNSUPPORTED_METHOD;
        return;
    }
}

void StatusHandler::onBody(std::unique_ptr<folly::IOBuf>) {
    // Do nothing, we only support GET
}

void StatusHandler::onEOM() {
    switch(m_err) {
        case HttpCode::E_UNSUPPORTED_METHOD:
            proxygen::ResponseBuilder(downstream_)
                .status(WebServiceUtils::to(HttpStatusCode::METHOD_NOT_ALLOWED),
                        WebServiceUtils::toString(HttpStatusCode::METHOD_NOT_ALLOWED))
                .sendWithEOM();
            return;
        default:
            break;
    }

    folly::dynamic vals = getStatus();
    ResponseBuilder(downstream_)
        .status(WebServiceUtils::to(HttpStatusCode::OK),
                WebServiceUtils::toString(HttpStatusCode::OK))
        .body(folly::toJson(vals))
        .sendWithEOM();
}

void StatusHandler::onUpgrade(proxygen::UpgradeProtocol) noexcept {
    // Do nothing
}


void StatusHandler::requestComplete() noexcept {
    delete this;
}


void StatusHandler::onError(ProxygenError error) noexcept {
    delete this;
}

folly::dynamic StatusHandler::getStatus() {
    folly::dynamic json = folly::dynamic::object();
    json["status"] = "running";
    return json;
}