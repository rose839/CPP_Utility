#ifndef COMMON_H
#define COMMON_H

#include <string>

enum class HttpCode {
    SUCCEEDED            = 0,
    E_UNSUPPORTED_METHOD = -1,
    E_UNPROCESSABLE      = -2,
    E_ILLEGAL_ARGUMENT   = -3,
};

enum class HttpStatusCode {
    OK                 = 200,
    BAD_REQUEST        = 400,
    FORBIDDEN          = 403,
    NOT_FOUND          = 404,
    METHOD_NOT_ALLOWED = 405,
};

static std::map<HttpStatusCode, std::string> statusStringMap {
    {HttpStatusCode::OK,                     "OK"},
    {HttpStatusCode::BAD_REQUEST,            "Bad Request"},
    {HttpStatusCode::FORBIDDEN,              "Forbidden"},
    {HttpStatusCode::NOT_FOUND,              "Not Found"},
    {HttpStatusCode::METHOD_NOT_ALLOWED,     "Method Not Allowed"}
};

class WebServiceUtils final {
public:
    static int32_t to(HttpStatusCode code) {
        return static_cast<int32_t>(code);
    }

    static std::string toString(HttpStatusCode code) {
        return statusStringMap[code];
    }
};

#endif