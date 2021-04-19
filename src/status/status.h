/**
 * @file  status.h
 *
 * Status is modeled on the one from levelDB, beyond that,
 * this one adds support on move semantics and formatted error messages.
 *
 * Status is as cheap as raw pointers in the successful case,
 * without any heap memory allocations.
 *
 * @date  2021-04-19
 */

#ifndef STATUS_H
#define STATUS_H

#include <memory>
#include <string>
#include <cstdint>

class Status final {
public:
    Status() = default;
    ~Status() = default;

    Status(const Status &rhs) {
        m_state = rhs.m_state == nullptr ? nullptr : copyState(rhs.m_state.get());
    }

    Status& operator=(const Status &rhs) {
        if (m_state == rhs.m_state) {
            m_state = rhs.m_state == nullptr ? nullptr : copyState(rhs.m_state.get());
        }
        return *this;
    }

    Status(Status &&rhs) noexcept {
        m_state = std::move(rhs.m_state);
    }

    Status& operator=(Status &&rhs) noexcept {
        if (m_state != rhs.m_state) {
            m_state = std::move(rhs.m_state);
        }
        return *this;
    }

    bool operator==(const Status &rhs) const {
        if (m_state == rhs.m_state) {
            return true;
        }
        return code() == rhs.code();
    }

    bool operator!=(const Status &rhs) const {
        return !(*this == rhs);
    }

     bool ok() const {
        return m_state == nullptr;
    }

    static Status OK() {
        return Status();
    }

#define STATUS_GENERATOR(ERROR)                 \
    static Status ERROR() {                     \
        return Status(k##ERROR, "");            \
    }                                           \
                                                \
    static Status ERROR(std::string msg) {      \
        return Status(k##ERROR, msg);           \
    }                                           \
                                                \
    static Status ERROR(const char *fmt, ...)   \
        __attribute__((format(printf, 1, 2))) { \
        va_list args;                           \
        va_start(args, fmt);                    \
        auto msg = format(fmt, args);           \
        va_end(args);                           \
        return Status(k##ERROR, msg);           \
    }                                           \
                                                \
    bool is##ERROR() const {                    \
        return code() == k##ERROR;              \
    }

    // General errors
    STATUS_GENERATOR(Error);
    STATUS_GENERATOR(NoSuchFile);
    STATUS_GENERATOR(NotSupported);

#undef STATUS_GENERATOR

    // If some kind of error really needs to be distinguished with others using a specific
    // code, other than a general code and specific msg, you could add a new code below,
    // e.g. kSomeError, and add the cooresponding STATUS_GENERATOR(SomeError)
    enum Code : uint16_t {
        // OK
        kOk                     = 0,
        // 1xx, for general errors
        kError                  = 101,
        kNoSuchFile             = 102,
        kNotSupported           = 103,
    };

    Code code() const {
        if (m_state == nullptr) {
            return kOk;
        }
        return reinterpret_cast<const Header*>(m_state.get())->m_code;
    }

private:
    uint16_t size() const {
        return reinterpret_cast<const Header*>(m_state.get())->m_size;
    }

    Status(Code code, std::string msg);

    static std::unique_ptr<const char[]> copyState(const char *state);

    static std::string format(const char *fmt, va_list args);

    static const char *toString(Code code);
private:
    struct Header {
        uint16_t m_size;
        Code m_code;
    };

    static constexpr auto headerSize = sizeof(Header);

    //    m_state == nullptr means ok.
    //    otherwise, the buffer layout is:
    //        state[0-1]:   length of the error message,size()-headerSize
    //        state[2-3]:   code
    //        state[4-...]: verbose error message
    std::unique_ptr<const char[]> m_state;
};

#endif