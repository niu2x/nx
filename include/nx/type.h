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
#include <variant>

namespace nx {

using ByteBuffer = std::vector<uint8_t>;

template <class T>
using Optional = std::optional<T>;

using String = std::string;

template <class... T>
using Variant = std::variant<T...>;

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

struct IO_Success {
    size_t bytes;
};

struct EndOfFile {
};

enum class IO_Error {
    NOT_OPEN,
    IO_FAIL,
};

using ReadResult = Variant<IO_Error, EndOfFile, IO_Success>;

class Read {
public:
    virtual ~Read() = 0;
    virtual ReadResult read(void* buffer, size_t bytes) = 0;
    // {
    //     (void)buffer;
    //     (void)bytes;
    //     return 0;
    // }
};

using WriteResult = Variant<IO_Error, IO_Success>;

class Write {
public:
    virtual ~Write() = 0;
    virtual WriteResult write(const void* buffer, size_t bytes) = 0;
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
