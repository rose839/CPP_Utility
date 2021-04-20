#include "status/status.h"

Status::Status(Code code, std::string msg) {
    const uint16_t size = msg.size();
    auto state = std::unique_ptr<char[]>(new char[size+headerSize]);
    auto *header = reinterpret_cast<Header*>(state.get());

    header->m_size = size;
    header->m_code = code;
    ::memcpy(&state[headerSize], msg.data(), size);
    m_state = std::move(state);
}

std::string Status::message() const {
    if (m_state == nullptr) {
        return "";
    }
    return std::string(&m_state[headerSize], size());
}

std::string Status::toString() const {
    Code code = this->code();
    if (code == kOk) {
        return "OK";
    }
    std::string result(toString(code));
    result.append(&m_state[headerSize], size());
    return result;
}

std::unique_ptr<const char[]> Status::copyState(const char *state) {
    const auto size = *reinterpret_cast<const uint16_t*>(state);
    const auto total = size + headerSize;
    auto result = std::unique_ptr<char[]>(new char[total]);
    ::memcpy(&result[0], state, total);
    return result;
}

std::string Status::format(const char *fmt, va_list args) {
    char result[256];
    vsnprintf(result, sizeof(result), fmt, args);
    return result;
}

// static
const char *Status::toString(Code code) {
    switch (code) {
        case kOk:
            return "OK";
        case kError:
            return "GeneralError: ";
        case kNoSuchFile:
            return "NoSuchFile: ";
        case kNotSupported:
            return "NotSupported: ";
    }

    return "";
}