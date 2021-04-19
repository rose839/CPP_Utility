#include "status/status.h"

Status::Status(Code code, std::string msg) {
    const uint16_t size = msg.size();
    auto state = std::unique_ptr<char[]>(new char[size+headerSize]);
    auto *header = reinterpret_cast<Header*>(state.get());

    header->m_size = size;
    header->m_code = code;
    ::memcpy(&state[headerSize], msg.data(), size);
}