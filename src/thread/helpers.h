#ifndef HELPERS_H
#define HELPERS_H

class NonCopable {
protected:
    NonCopable();
    NonCopable(const NonCopable&) = delete;
    NonCopable& operator=(const NonCopable&) = delete;
};

class NonMovable {
protected:
    NonMovable() {}
    NonMovable(NonMovable&&) = delete;
    NonMovable& operator=(NonMovable&&) = delete;
};

#endif