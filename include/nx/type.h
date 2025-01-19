#pragma once

#include <string.h>
#include <stdarg.h>

#include <cstddef>
#include <cstdint>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <memory>
#include <optional>
#include <variant>
#include <chrono>
#include <string>
#include <stdexcept>

namespace nx {

using ByteBuffer = std::vector<uint8_t>;

template <class T>
using Optional = std::optional<T>;

using String = std::string;

/**
 * @brief      用于被private继承, 子类将不可被Copy, 不可被Move
 */
class Uncopyable {
public:
    Uncopyable() = default;
    ~Uncopyable() = default;

    Uncopyable(const Uncopyable&) = delete;
    Uncopyable& operator=(const Uncopyable&) = delete;

    Uncopyable(Uncopyable&&) = delete;
    Uncopyable& operator=(Uncopyable&&) = delete;
};

class RuntimeException : public std::exception {
public:
    explicit RuntimeException(const std::string& message)
    : message_(message) { }
    const char* what() const noexcept override { return message_.c_str(); }

private:
    std::string message_;
};

void panic_fmt(const char* fmt, ...);

constexpr size_t operator""_kb(unsigned long long int n) { return n * 1024; }

template <class FROM, class TO>
FROM from(TO);

template <class T>
String to_string(const T&);

enum class OpenMode {
    WRITE,
    READ,
};

class Read {
public:
    virtual ~Read() = 0;
    virtual size_t read(void* buffer, size_t bytes)
    {
        (void)buffer;
        (void)bytes;
        return 0;
    }
};

} // namespace nx

/**
 * @brief      panic with a fatal error message.
 *
 * @param      msg   The message
 */
#define NX_PANIC(msg, ...) nx::panic_fmt(msg, ##__VA_ARGS__)

#define NX_ASSERT(cond, msg, ...)                                              \
    if (!(cond)) {                                                             \
        NX_PANIC(msg, ##__VA_ARGS__);                                          \
    }
