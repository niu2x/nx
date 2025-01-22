#pragma once

#include <string.h>
#include <stdarg.h>

#include <cstddef>
#include <cstdint>
#include <vector>
#include <map>
#include <type_traits>
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

template <class T>
using Vector = std::vector<T>;

using String = std::string;

template <class... T>
using Variant = std::variant<T...>;

template <class T>
using UniquePtr = std::unique_ptr<T>;

template <class T>
using SharedPtr = std::shared_ptr<T>;

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
    bool read_exact(void* buffer, size_t bytes);
};

using WriteResult = Variant<IO_Error, IO_Success>;

class Write {
public:
    virtual ~Write() = 0;
    virtual WriteResult write(const void* buffer, size_t bytes) = 0;
    virtual bool write_all(const void* buffer, size_t bytes);
};

bool pipe(Read& reader, Write& writer);

class MemoryFile : public Read, private Uncopyable {
public:
    MemoryFile(const uint8_t* buffer, size_t buf_len);
    ReadResult read(void* buffer, size_t bytes) override;

private:
    const uint8_t* buffer_;
    size_t buf_len_;
    size_t read_pos_;
};

inline uint8_t ceil_pow2(uint8_t n)
{
    n -= 1;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    return n + 1;
}

inline uint16_t ceil_pow2(uint16_t n)
{
    n -= 1;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    return n + 1;
}

inline uint32_t ceil_pow2(uint32_t n)
{
    n -= 1;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    return n + 1;
}

inline uint64_t ceil_pow2(uint64_t n)
{
    n -= 1;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n |= n >> 32;
    return n + 1;
}

inline bool is_pow2(uint8_t x) { return ((x - 1) & x) == 0; }

inline bool is_pow2(uint16_t x) { return ((x - 1) & x) == 0; }

inline bool is_pow2(uint32_t x) { return ((x - 1) & x) == 0; }

inline bool is_pow2(uint64_t x) { return ((x - 1) & x) == 0; }

class CRC32 {
public:
    CRC32();

    void update(const uint8_t* buf, size_t len);
    uint32_t get_value() const;

private:
    uint32_t table_[256];
    uint32_t initial_;
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

#define NX_GET_BIT_BOOL(byte, n) (NX_GET_BIT((byte), (n)) == 1)
#define NX_GET_BIT(byte, n)      (((byte) >> n) & 1)
